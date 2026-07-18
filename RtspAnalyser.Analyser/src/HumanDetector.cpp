#include <thread>
#include <atomic>
#include <vector>

#include <opencv2/opencv.hpp>
#include <onnxruntime/onnxruntime_cxx_api.h>
#include <oneapi/tbb/concurrent_queue.h>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/HumanDetector.h"
#include "DelNico/RtspAnalyser/Analyser/Streamer.h"
#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCaller.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCalling.h"

using namespace DelNico::RtspAnalyser::Analyser;
using namespace DelNico::RtspAnalyser::Libs;

HumanDetector::HumanDetector(
    oneapi::tbb::concurrent_queue<cv::Mat> & frames,
    Motion::MotionManager * motionManager,
    std::vector<cv::Rect> zones,
    float confidence_threshold,
    bool onCPU
) :
    cond(),
    isEnabled(false),
    thread(),
    frames(frames),
    motionManager(motionManager),
    zones(zones),
    confidence_threshold(confidence_threshold),
    streamer(nullptr),
    human_detected_output(nullptr)
{
    env = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "YOLOv26_Detector");
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(4);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    std::string model_path = "/home/nico/project/RTSP-Analyser/iamodel/yolov26n.onnx";
    session = std::make_unique<Ort::Session>(*env, model_path.c_str(), session_options);
    input_names = {"images"};
    output_names = {"output0"};
}

HumanDetector::~HumanDetector()
{
    stop();
}

void HumanDetector::setStreamer(Streamer * streamer, oneapi::tbb::concurrent_queue<cv::Mat> * human_detected_output)
{
    this->streamer = streamer;
    this->human_detected_output = human_detected_output;
}

void HumanDetector::start()
{
    isEnabled.store(true);
    thread = std::thread(&HumanDetector::run, this);
}

void HumanDetector::stop()
{
    if(thread.joinable())
    {
        isEnabled.store(false);
        notify();
        thread.join();
    }
}

void HumanDetector::notify()
{
    cond.notify();
}

void HumanDetector::run()
{
    cv::Mat frame;
    std::tuple<bool, cv::Mat, float> result;
    while (isEnabled)
    {
        cond.wait();
        if(! frames.try_pop(frame))
            continue;

        try
        {
            result = isHumanDetected(frame, true);
        }
        catch(const std::exception & e)
        {
            Logger::log_main("HumanDetector::run - Exception: " + std::string(e.what()));
            continue;
        }
        if(std::get<0>(result))
        {
            motionManager->notify(
                Motion::MotionManagerCalling(
                    Motion::MotionManagerCaller::HUMAN_DETECTOR,
                    true,
                    ((int) (std::get<2>(result) * 100.0f))             // avoid format error of float so let do in pourcentage
                )
            );
        }
        if(streamer)
        {
            human_detected_output->push(std::get<1>(result));
            streamer->notify();
        }
    }
}

std::tuple<bool, cv::Mat, float> HumanDetector::isHumanDetected(const cv::Mat & frame, const bool need_output) const
{
    cv::Mat output;
    cv::resize(frame, output, cv::Size(frame.cols / 2, frame.rows / 2));

    if (frame.empty()) {
        return std::make_tuple(false, output, 0.0f);
    }

    // 1. Prétraitement de l'image (YOLO attend du 640x640, RGB, normalisé)
    cv::Mat resized_img;
    cv::Size target_size(640, 640);
    cv::resize(frame, resized_img, target_size);
    cv::cvtColor(resized_img, resized_img, cv::COLOR_BGR2RGB);

    // Convertir l'image en Float 32 et normaliser (1/255.0)
    cv::Mat float_img;
    resized_img.convertTo(float_img, CV_32FC3, 1.0 / 255.0);

    // Réarranger les canaux de HWC (OpenCV) à CHW (ONNX standard)
    std::vector<float> input_tensor_values(1 * 3 * 640 * 640);
    std::vector<cv::Mat> chw_channels(3);
    for (int i = 0; i < 3; ++i) {
        chw_channels[i] = cv::Mat(target_size, CV_32FC1, &input_tensor_values[i * 640 * 640]);
    }
    cv::split(float_img, chw_channels);

    // 2. Création des Tensors ONNX Runtime
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    std::array<int64_t, 4> input_shape = {1, 3, 640, 640};

    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info, input_tensor_values.data(), input_tensor_values.size(), input_shape.data(), input_shape.size()
    );

    // 3. Inférence (forward)
    auto output_tensors = session->Run(
        Ort::RunOptions{nullptr},
        input_names.data(), &input_tensor, 1,
        output_names.data(), 1
    );

    // 4. Post-traitement des données reçues (Format YOLOv26: [1 x 300 x 6])
    float* raw_output = output_tensors[0].GetTensorMutableData<float>();
    auto output_shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();

    int num_detections = output_shape[1]; // Devrait être 300
    int num_elements = output_shape[2];   // Devrait être 6 [x1, y1, x2, y2, score, class_id]

    bool human_found = false;
    float max_score = 0.0f;

    float scale_x = static_cast<float>(output.cols) / 640.0f;
    float scale_y = static_cast<float>(output.rows) / 640.0f;

    for (int i = 0; i < num_detections; ++i) {
        int index = i * num_elements;

        float confidence = raw_output[index + 4];
        int class_id     = static_cast<int>(raw_output[index + 5]);

        // Classe 0 = Personne dans le modèle COCO standard
        if (class_id == 0 && confidence >= confidence_threshold) {

            float x1 = raw_output[index + 0];
            float y1 = raw_output[index + 1];
            float x2 = raw_output[index + 2];
            float y2 = raw_output[index + 3];

            int left   = static_cast<int>(x1 * scale_x);
            int top    = static_cast<int>(y1 * scale_y);
            int right  = static_cast<int>(x2 * scale_x);
            int bottom = static_cast<int>(y2 * scale_y);

            int width  = right - left;
            int height = bottom - top;

            int center_x = left + (width / 2);
            int center_y = top + (height / 2);

            if (isHumanInsideZone(center_x, center_y)) {
                human_found = true;
                if (confidence > max_score) {
                    max_score = confidence;
                }

                if (need_output) {
                    cv::rectangle(output, cv::Rect(left, top, width, height), cv::Scalar(0, 255, 0), 2);
                } else {
                    break;
                }
            }
        }
    }

    return std::make_tuple(human_found, output, max_score);
}

bool HumanDetector::isHumanInsideZone(int x_center, int y_center) const 
{
    for(auto & zone : zones)
    {
        if(zone.x < x_center < (zone.x + zone.width))
        {
            if(zone.y < y_center < (zone.y + zone.height))
                return true;
        }
    }
    return false;
}

bool HumanDetector::operator==(const HumanDetector & other) const
{
    return this == &other;
}
