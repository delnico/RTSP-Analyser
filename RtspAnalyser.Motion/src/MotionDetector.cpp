#include <thread>
#include <list>
#include <atomic>
#include <deque>
#include <memory>
#include <vector>
#include <cstdint>
#include <cmath>

// to remove
#include <iostream>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Motion/MotionDetector.h"

using namespace Nico::RtspAnalyser::Motion;

MotionDetector::MotionDetector(
    std::deque<cv::Mat> & frames,
    std::deque<cv::Mat> & fgMasks,
    int64_t ms_one_frame
) :
    cond(),
    isEnabled(false),
    zones(),
    frames(frames),
    fgMasks(fgMasks),
    viewer(nullptr),
    cv_motion_history(500),
    cv_motion_var_threshold(60),
    cv_motion_detect_shadows(false),
    ms_one_frame(ms_one_frame),
    ms_one_frame_original(ms_one_frame),
    frame_skipping(0),
    frames_count(0)
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

    while (isEnabled)
    {
        motionDetected = false;
        wait();
        if(frames.size() < 2)
            continue;
        frame = frames.front();
        frames.pop_front();

        if(frame_skipping != 0) {
            if(frames_count % frame_skipping != 0)
                continue;
        }
        frames_count++;

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
        {
            std::lock_guard<Nico::RtspAnalyser::Libs::Spinlock> lock(slock_processing_times);
            processing_times.push_back(elapsed.count());
        }
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

void MotionDetector::watchdog() {
    std::lock_guard<Nico::RtspAnalyser::Libs::Spinlock> lock(slock_processing_times);
    auto size = processing_times.size();
    if(size > 10) {
        int64_t sum = 0;
        for(auto & time : processing_times) {
            sum += time;
        }
        auto avg = sum / size;
        if(avg > ms_one_frame) {
            frame_skipping = std::ceil(avg / ms_one_frame);
            ms_one_frame = ms_one_frame * frame_skipping;
            std::cout << "frame_skipping: " << frame_skipping << " ms frame :" << ms_one_frame << std::endl;
        }
        else {
            if(avg < ms_one_frame_original) {
                frame_skipping = 0;
                ms_one_frame = ms_one_frame_original;
                std::cout << "frame_skipping: " << frame_skipping << " ms frame :" << ms_one_frame << std::endl;
            }
        }
        processing_times.clear();
    }
}
