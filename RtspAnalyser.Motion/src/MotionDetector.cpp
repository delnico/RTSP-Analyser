#include <thread>
#include <list>
#include <atomic>
#include <deque>
#include <memory>

// to remove
#include <iostream>
#include <chrono>
#include <cstdint>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Motion/MotionDetector.h"

using namespace Nico::RtspAnalyser::Motion;

MotionDetector::MotionDetector(std::deque<cv::Mat> & frames, std::deque<cv::Mat> & fgMasks) :
    cond(),
    isEnabled(false),
    zones(),
    frames(frames),
    fgMasks(fgMasks),
    viewer(nullptr),
    cv_motion_history(500),
    cv_motion_var_threshold(60),
    cv_motion_detect_shadows(false)
{
    // zone depend of screen resolution and scale
    zones.push_back(cv::Rect(0, 150, 550, 210));
    zones.push_back(cv::Rect(250, 50, 325, 100));
}

MotionDetector::~MotionDetector() {}

void MotionDetector::start() {
    if(! isEnabled.load()) {
        isEnabled.store(true);
        thread = std::thread(&MotionDetector::run, this);
    }
}

void MotionDetector::stop() {
    if(thread.joinable()) {
        isEnabled.store(false);
        notify();
        thread.join();
    }
}

void MotionDetector::setViewer(Nico::RtspAnalyser::Analyser::Viewer * viewer) {
    this->viewer = viewer;
}

void MotionDetector::run() {
    cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::createBackgroundSubtractorMOG2(cv_motion_history, cv_motion_var_threshold, cv_motion_detect_shadows);

    cv::Mat frame, fgMask, roiMask, grayFrame;

    bool motionDetected = false;
    int64_t tooMuschTime = 0;

    std::vector<int64_t> times;

    while (isEnabled)
    {
        motionDetected = false;
        wait();
        if(frames.size() < 2)
            continue;
        frame = frames.front();
        frames.pop_front();

        auto start = std::chrono::steady_clock::now();

        cv::resize(frame, frame, cv::Size(frame.cols / 2, frame.rows / 2));
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        bgSubtractor->apply(frame, fgMask);
        roiMask = cv::Mat::zeros(fgMask.size(), fgMask.type());

        for(const auto & zone : zones) {
            fgMask(zone).copyTo(roiMask(zone));

            // remove small area - noise
            cv::erode(roiMask(zone), roiMask(zone), cv::Mat(), cv::Point(-1, -1), 1);
            cv::dilate(roiMask(zone), roiMask(zone), cv::Mat(), cv::Point(-1, -1), 1);

            cv::GaussianBlur(roiMask(zone), roiMask(zone), cv::Size(5, 5), 0);

            std::vector<std::vector<cv::Point>> outlines;
            cv::findContours(roiMask(zone), outlines, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


            if(outlines.size() > 0) {
                motionDetected = true;
            }

            if(viewer != nullptr) {
                for(size_t i = 0; i < outlines.size(); i++) {
                    cv::Rect rect = cv::boundingRect(outlines[i]);
                    if(rect.area() > 500) {
                        cv::rectangle(frame, rect, cv::Scalar(0, 0, 255), 2);
                    }
                }

                cv::rectangle(frame, zone, cv::Scalar(0, 255, 0), 2);
            }
        }

        if(motionDetected) {
            // trigger event to MotionManager thread
        }

        if(viewer != nullptr) {
            fgMasks.push_back(cv::Mat(roiMask));
            viewer->notify();
        }

        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if(elapsed.count() > 33) {
            tooMuschTime++;
            std::cout << "MotionDetector too much " << tooMuschTime << " time: " << elapsed.count() << std::endl;
        }
        times.push_back(elapsed.count());
    }

    int64_t average_time = 0;
    int64_t max_time = 0;
    int64_t min_time = 1000;
    int64_t sum_time = 0;
    int64_t median_time = 0;
    for(const auto & time : times) {
        average_time += time;
        sum_time += time;
        if(time > max_time)
            max_time = time;
        if(time < min_time)
            min_time = time;
    }
    average_time /= times.size();
    std::sort(times.begin(), times.end());
    median_time = times.at(times.size() / 2);

    std::cout << "MotionDetector average time: " << average_time << std::endl;
    std::cout << "MotionDetector max time: " << max_time << std::endl;
    std::cout << "MotionDetector min time: " << min_time << std::endl;
    std::cout << "MotionDetector median time: " << median_time << std::endl;

}

void MotionDetector::notify() {
    cond.notify();
}

void MotionDetector::wait() {
    cond.wait();
}

bool MotionDetector::operator==(const MotionDetector & other) const
{
    return &other == this;
}
