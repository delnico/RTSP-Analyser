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
    cv_motion_history(1800),
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
    cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::createBackgroundSubtractorKNN();

    cv::Mat frame, fgMask, roiMask, grayFrame;

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
