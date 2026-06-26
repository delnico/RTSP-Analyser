#include <thread>
#include <atomic>
#include <deque>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/HumanDetector.h"
#include "DelNico/RtspAnalyser/Analyser/Streamer.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCaller.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCalling.h"

using namespace DelNico::RtspAnalyser::Analyser;
using namespace DelNico::RtspAnalyser::Libs;

HumanDetector::HumanDetector(std::deque<cv::Mat> & frames, Motion::MotionManager * motionManager) :
    cond(),
    isEnabled(false),
    thread(),
    frames(frames),
    net(),
    motionManager(motionManager),
    streamer(nullptr),
    human_detected_output(nullptr)
{
    net = cv::dnn::readNetFromONNX("/home/nico/project/RTSP-Analyser/yolov8n.onnx");
    // for use in CPU mode
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
}

HumanDetector::~HumanDetector()
{
    stop();
}

void HumanDetector::setStreamer(Streamer * streamer, std::deque<cv::Mat> * human_detected_output)
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
    std::tuple<bool, cv::Mat> result;
    while (isEnabled)
    {
        cond.wait();
        if(frames.empty())
            continue;
        frame = frames.front();
        frames.pop_front();
        result = isHumanDetected(frame, true);
        if(std::get<0>(result))
        {
            motionManager->notify(
                Motion::MotionManagerCalling(
                    Motion::MotionManagerCaller::HUMAN_DETECTOR,
                    true
                )
            );
        }
        if(streamer)
        {
            human_detected_output->push_back(std::get<1>(result));
            streamer->notify();
        }
    }
}

std::tuple<bool, cv::Mat> HumanDetector::isHumanDetected(const cv::Mat & frame, const bool need_output) const
{
    cv::Mat blob, output;
    cv::resize(frame, output, cv::Size(frame.cols / 2, frame.rows / 2));

    if (frame.empty()) {
        return std::make_tuple(false, output);
    }

    // 1    prepare image for YOLO (640x640, normalization 1/255.0, BGR to RGB)
    cv::dnn::blobFromImage(frame, blob, 1.0 / 255.0, cv::Size(640, 640), cv::Scalar(), true, false);
    
    //  non-const reference to net to call setInput and forward, to avoid modifying the original one
    auto & non_const_net = const_cast<cv::dnn::Net&>(net);
    non_const_net.setInput(blob);

    // 2    predict
    std::vector<cv::Mat> outputs;
    non_const_net.forward(outputs, non_const_net.getUnconnectedOutLayersNames());

    cv::Mat raw_output = outputs[0];
    
    // remodeling output of YOLO [1, 84, 8400] -> [8400, 84]
    if (raw_output.dims == 3) {
        raw_output = raw_output.reshape(1, raw_output.size[1]);
        cv::transpose(raw_output, raw_output);
    }

    bool human_found = false;
    const float confidence_threshold = 0.50f;

    for (int i = 0; i < raw_output.rows; ++i) {
        float person_score = raw_output.at<float>(i, 4);    // 4 is person class score in YOLOv8 output

        if (person_score >= confidence_threshold) {
            human_found = true;

            if (need_output) {
                float cx = raw_output.at<float>(i, 0);
                float cy = raw_output.at<float>(i, 1);
                float w  = raw_output.at<float>(i, 2);
                float h  = raw_output.at<float>(i, 3);

                float scale_x = static_cast<float>(output.cols) / 640.0f;   // yolov8n input size is 640x640
                float scale_y = static_cast<float>(output.rows) / 640.0f;

                int left   = static_cast<int>((cx - w / 2.0f) * scale_x);
                int top    = static_cast<int>((cy - h / 2.0f) * scale_y);
                int width  = static_cast<int>(w * scale_x);
                int height = static_cast<int>(h * scale_y);

                cv::rectangle(output, cv::Rect(left, top, width, height), cv::Scalar(0, 255, 0), 2);
            }
            
            if (!need_output) {
                break;
            }
        }
    }

    return std::make_tuple(human_found, output);
}

bool HumanDetector::operator==(const HumanDetector & other) const
{
    return this == &other;
}
