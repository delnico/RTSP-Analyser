#include <thread>
#include <list>
#include <atomic>
#include <deque>
#include <memory>

// to remove
#include <iostream>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Motion/MotionDetector.h"

using namespace Nico::RtspAnalyser::Analyser;

MotionDetector::MotionDetector(std::deque<cv::Mat> & frames) :
    cond(),
    isEnabled(ATOMIC_FLAG_INIT),
    zones(),
    frames(frames),
    cv_motion_history(500),
    cv_motion_var_threshold(60),
    cv_motion_detect_shadows(false)
{
    zones.push_back(cv::Rect(0, 300, 1100, 420));
    zones.push_back(cv::Rect(500, 100, 650, 200));
}

MotionDetector::~MotionDetector() {}

void MotionDetector::start() {
    if(! isEnabled.test_and_set(std::memory_order_acquire))
        thread = std::thread(&MotionDetector::run, this);
}

void MotionDetector::stop() {
    if(thread.joinable()) {
        isEnabled.clear(std::memory_order_release);
        notify();
        thread.join();
    }
}

void MotionDetector::run() {
    cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::createBackgroundSubtractorMOG2(cv_motion_history, cv_motion_var_threshold, cv_motion_detect_shadows);

    cv::Mat frame, fgMask, roiMask, grayFrame;

    bool motionDetected = false;
    bool displayZone = false;
    while (isEnabled.test())
    {
        motionDetected = false;
        wait();
        if(frames.empty())
            continue;
        frame = frames.front();
        frames.pop_front();

        auto start = std::chrono::high_resolution_clock::now();


        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        bgSubtractor->apply(grayFrame, fgMask);
        roiMask = cv::Mat::zeros(fgMask.size(), fgMask.type());

        for(const auto & zone : zones) {
            fgMask(zone).copyTo(roiMask(zone));

            // remove small area - noise
            cv::erode(roiMask(zone), roiMask(zone), cv::Mat(), cv::Point(-1, -1), 1);
            cv::dilate(roiMask(zone), roiMask(zone), cv::Mat(), cv::Point(-1, -1), 1);

            cv::GaussianBlur(roiMask(zone), roiMask(zone), cv::Size(15, 15), 0);

            std::vector<std::vector<cv::Point>> outlines;
            cv::findContours(roiMask(zone), outlines, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


            if(outlines.size() > 0) {
                motionDetected = true;
            }

            if(displayZone) {
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

        if(displayZone) {
            // send roiMask to Viewer thread via queue
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "MotionDetector Elapsed time: " << elapsed.count() << " ms" << std::endl;
    }
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
