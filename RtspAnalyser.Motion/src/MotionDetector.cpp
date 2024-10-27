#include <thread>
#include <list>
#include <atomic>
#include <deque>
#include <memory>
#include <vector>
#include <cstdint>
#include <cmath>
#include <format>
#include <chrono>
#include <cmath>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Motion/MotionDetector.h"

using namespace Nico::RtspAnalyser::Motion;

MotionDetector::MotionDetector(
    std::deque<cv::Mat> & frames,
    std::deque<cv::Mat> & fgMasks,
    int64_t fps,
    int64_t frame_skipping
) :
    cond(),
    isEnabled(false),
    zones(),
    frames(frames),
    fgMasks(fgMasks),
    viewer(nullptr),
    cv_motion_history(5),
    cv_motion_var_threshold(1024),
    cv_motion_detect_shadows(false),
    ms_one_frame(1000LL / fps),
    ms_one_frame_original(1000LL / fps),
    fps(fps),
    frame_skipping(frame_skipping),
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
    //cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::createBackgroundSubtractorMOG2(cv_motion_history, cv_motion_var_threshold, cv_motion_detect_shadows);
    //cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::createBackgroundSubtractorKNN();

    cv::Mat frame, grayFrame, avgFrame, diffFrame, thresholdFrame;

    std::deque<cv::Mat> previousFrames;

    bool motionDetected = false;
    int64_t tooMuschTime = 0;

    while (isEnabled)
    {
        motionDetected = false;
        wait();

        try {
            if(frames.size() < 2)
                continue;
            frame = frames.front();
            frames.pop_front();
        }
        catch(const std::exception & e) {
            // call logger
            continue;
        }
        
        if(frame.empty() || frame.size().width == 0 || frame.size().height == 0)
            continue;

        if(frame_skipping > 1) {
            if(frames_count % (frame_skipping-1) != 0)
                continue;
        }
        frames_count++;

        auto start = std::chrono::steady_clock::now();

        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        if(previousFrames.size() > cv_motion_history)
            previousFrames.pop_front();
        
        previousFrames.push_back(grayFrame);

        if(previousFrames.size() == cv_motion_history) {
            avgFrame = cv::Mat::zeros(grayFrame.size(), CV_32F);

            for(auto & previousFrame : previousFrames) {
                cv::Mat floatFrame;
                previousFrame.convertTo(floatFrame, CV_32F);
                avgFrame += floatFrame;
            }

            avgFrame /= cv_motion_history;

            cv::Mat avgFrame8U;
            avgFrame.convertTo(avgFrame8U, CV_8U);

            cv::absdiff(grayFrame, avgFrame8U, diffFrame);

            cv::threshold(diffFrame, thresholdFrame, 25, 255, cv::THRESH_BINARY);

            motionDetected = cv::countNonZero(thresholdFrame) > 0;
        }
        

        if(motionDetected) {
            // trigger event to MotionManager thread
        }

        if(viewer != nullptr) {
            if(thresholdFrame.size().width > 0 && thresholdFrame.size().height > 0)
            {
                fgMasks.push_back(cv::Mat(thresholdFrame));
                viewer->notify();
            }
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

std::string MotionDetector::watchdog() {
    std::string result = "";
    std::lock_guard<Nico::RtspAnalyser::Libs::Spinlock> lock(slock_processing_times);
    auto size = processing_times.size();
    if(size > fps) {    // > ~ 1s
        int64_t sum = 0;
        for(auto & time : processing_times) {
            sum += time;
        }
        double max = *std::max_element(processing_times.begin(), processing_times.end());
        double ratio = max / (double)ms_one_frame;
        double ratioround = std::ceil(ratio);
        int64_t tmp = frame_skipping;
        /*if(max > ms_one_frame) {
            tmp++;
            ms_one_frame = ms_one_frame + ms_one_frame_original;
        }
        else if(max < (ms_one_frame - ms_one_frame_original) && frame_skipping > 1) {
            tmp--;
            ms_one_frame = ms_one_frame - ms_one_frame_original;
        }

        if(tmp != frame_skipping) {
            frame_skipping = tmp;
        }*/
        processing_times.clear();
        result = std::format("WATCHDOG : MotionDetector : Frame skipping at {}, frame ms at {}, max = {}, ratio = {} ratioround = {}", frame_skipping, ms_one_frame, max, ratio, ratioround);
    }
    return result;
}
