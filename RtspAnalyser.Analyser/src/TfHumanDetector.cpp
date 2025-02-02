#include <thread>
#include <atomic>
#include <deque>
#include <vector>
#include <fstream>

#include <opencv2/opencv.hpp>

#include <tensorflow/core/public/session.h>
#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/framework/graph.pb.h>

#include "Nico/RtspAnalyser/Analyser/TfHumanDetector.h"
#include "Nico/RtspAnalyser/Libs/Config.h"

using namespace Nico::RtspAnalyser::Analyser;

TfHumanDetector::TfHumanDetector(
    std::deque<cv::Mat> & input_frames,
    Libs::Config & config
) :
    isEnabled(false),
    input_frames(input_frames),
    config(config),
    model_path(config.get<std::string>("mobilenet_v2_model_path"))
{}

TfHumanDetector::~TfHumanDetector() {
    stop();
}

void TfHumanDetector::start() {
    if(! isEnabled.load()) {
        isEnabled.store(true);
        //load_model_labels();

        status = tensorflow::NewSession(tensorflow::SessionOptions(), &session);
        if(! status.ok())
            throw std::runtime_error("Could not create tensorflow session");
        status = tensorflow::ReadBinaryProto(tensorflow::Env::Default(), model_path, &graph_def);
        if(! status.ok())
            throw std::runtime_error("Could not read model file : \n \t" + status.error_message());
        status = session->Create(graph_def);
        if(! status.ok())
            throw std::runtime_error("Could not create graph");

        thread = std::thread(&TfHumanDetector::run, this);
    }
}

void TfHumanDetector::stop() {
    if(isEnabled.load()) {
        isEnabled.store(false);
        thread.join();
        delete session;
    }
}

void TfHumanDetector::notify() {
    input_cond.notify();
}

void TfHumanDetector::run() {
    while(isEnabled.load()) {
        input_cond.wait();
        if(input_frames.size() == 0)
            continue;
        cv::Mat frame = input_frames.front();
        input_frames.pop_front();
        detect(frame);
    }
}

void TfHumanDetector::detect(cv::Mat & frame) {
    cv::Mat frame_rgb;
    cv::cvtColor(frame, frame_rgb, cv::COLOR_BGR2RGB);
    cv::resize(frame_rgb, frame_rgb, cv::Size(224, 224));
    tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, 224, 224, 3}));
    auto input_tensor_mapped = input_tensor.tensor<float, 4>();
    for(int y = 0; y < 224; y++) {
        for(int x = 0; x < 224; x++) {
            cv::Vec3b pixel = frame_rgb.at<cv::Vec3b>(y, x);
            input_tensor_mapped(0, y, x, 0) = pixel[0] / 255.0;
            input_tensor_mapped(0, y, x, 1) = pixel[1] / 255.0;
            input_tensor_mapped(0, y, x, 2) = pixel[2] / 255.0;
        }
    }
    std::vector<tensorflow::Tensor> outputs;
    tensorflow::Status status = session->Run(
        {{ "input", input_tensor }},
        { "MobilenetV2/Predictions/Reshape_1" },
        {},
        &outputs
    );

    if(! status.ok())
        throw std::runtime_error("Could not run session");

    auto score = outputs[0].tensor<float, 2>();
    int max_index = std::max_element(score.data(), score.data() + score.size()) - score.data();
    //std::cout << "Detected: " << model_labels[max_index] << std::endl;
    //std::cout.flush();
}

void TfHumanDetector::load_model_labels() {
    auto labels_file_path = config.get<std::string>("labels_file_path");
    std::ifstream labels_file(labels_file_path);
    if(! labels_file.is_open())
        throw std::runtime_error("Could not open labels file");

    // read labels line by line
    std::string line;
    while(std::getline(labels_file, line)) {
        model_labels.push_back(line);
    }
    labels_file.close();
}

