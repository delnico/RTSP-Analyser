#include <thread>
#include <atomic>
#include <deque>
#include <vector>
#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/TfHumanDetector.h"
#include "Nico/RtspAnalyser/Libs/Config.h"

using namespace Nico::RtspAnalyser::Analyser;

TfHumanDetector::TfHumanDetector(
    std::deque<cv::Mat> & input_frames,
    Libs::Config & config
) :
    isEnabled(false),
    thread(),
    input_frames(input_frames),
    input_cond(),
    config(config)
{}

TfHumanDetector::~TfHumanDetector() {
    stop();
}

void TfHumanDetector::start() {
    if(! isEnabled.load()) {
        isEnabled.store(true);
        thread = std::thread(&TfHumanDetector::run, this);
    }
}

void TfHumanDetector::stop() {
    if(isEnabled.load()) {
        isEnabled.store(false);
        thread.join();
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

void TfHumanDetector::detect(cv::Mat frame) {
    // TODO
    // classify frame with MobilenetV2
    // check if contains person
    // if yes, notify MotionManager
}

void TfHumanDetector::load_model_labels() {
    // TODO
    // load labels from file
    // labels are in the format: "0: person", "1: bicycle", ...
    // store them in model_labels

    // get file path from config
    auto labels_file_path = config.get<std::string>("labels_file_path");
}

