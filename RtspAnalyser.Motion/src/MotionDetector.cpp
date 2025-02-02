#include <thread>
#include <list>
#include <atomic>
#include <deque>
#include <memory>
#include <vector>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <cmath>

#include <opencv2/opencv.hpp>
#include <opencv2/bgsegm.hpp>

#include <fmt/core.h>

#include "Nico/RtspAnalyser/Analyser/Viewer.h"
#include "Nico/RtspAnalyser/Libs/Config.h"
#include "Nico/RtspAnalyser/Libs/Logger.h"
#include "Nico/RtspAnalyser/Motion/MotionDetector.h"
#include "Nico/RtspAnalyser/Motion/MotionManager.h"

using namespace Nico::RtspAnalyser::Motion;

MotionDetector::MotionDetector(
    Libs::Config & config,
    std::deque<cv::Mat> & frames,
    std::deque<cv::Mat> & fgMasks,
    int64_t fps
) :
    isEnabled(false),
    frames(frames),
    fgMasks(fgMasks),
    viewer(nullptr),
    motionManager(nullptr),
    cv_motion_history(config.get<int>("opencv_model_history")),
    cv_motion_var_threshold(config.get<int>("opencv_model_var_threshold")),
    cv_motion_detect_shadows(config.get<bool>("opencv_model_detect_shadows")),
    ms_one_frame(1000LL / fps),
    ms_one_frame_original(1000LL / fps),
    fps(fps)
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

void MotionDetector::setViewer(Analyser::Viewer * viewer) {
    this->viewer = viewer;
}

void MotionDetector::setMotionManager(MotionManager * motionManager) {
    this->motionManager = motionManager;
}

void MotionDetector::run() {
    // cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::createBackgroundSubtractorMOG2(cv_motion_history, cv_motion_var_threshold, cv_motion_detect_shadows);

    cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::bgsegm::createBackgroundSubtractorCNT(6, false, 900, false);

    cv::Mat frame, fgMask, roiMask, grayFrame;

    bool motionDetected = false;

    while (isEnabled)
    {
        motionDetected = false;
        cond.wait();

        try {
            if(frames.size() < 2)
                continue;
            frame = frames.front();
            frames.pop_front();
        }
        catch(const std::exception & e) {
            Libs::Logger::log_main(fmt::format("MotionDetector::run error : {}", e.what()));
            continue;
        }
        
        if(frame.empty() || frame.size().width == 0 || frame.size().height == 0)
            continue;

        auto start = std::chrono::steady_clock::now();
        
        resize(frame, frame, cv::Size(frame.cols / 2, frame.rows / 2));
        cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        bgSubtractor->apply(frame, fgMask);
        roiMask = cv::Mat::zeros(fgMask.size(), fgMask.type());

        for(const auto & zone : zones) {
            fgMask(zone).copyTo(roiMask(zone));

            // remove small area - noise
            erode(roiMask(zone), roiMask(zone), cv::Mat(), cv::Point(-1, -1), 1);
            dilate(roiMask(zone), roiMask(zone), cv::Mat(), cv::Point(-1, -1), 1);

            GaussianBlur(roiMask(zone), roiMask(zone), cv::Size(5, 5), 0);

            std::vector<std::vector<cv::Point>> outlines;
            findContours(roiMask(zone), outlines, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


            if(outlines.size() > 0) {
                motionDetected = true;
            }

            if(viewer != nullptr) {
                for(const auto & outline : outlines) {
                    cv::Rect rect = boundingRect(outline);
                    if(rect.area() > 500) {
                        rectangle(frame, rect, cv::Scalar(0, 0, 255), 2);
                    }
                }

                rectangle(frame, zone, cv::Scalar(0, 255, 0), 2);
            }
        }
        

        if(motionDetected) {
            // trigger event to MotionManager thread
            if(motionManager != nullptr) {
                motionManager->notify();
            }
        }

        if(viewer != nullptr) {
            fgMasks.push_back(cv::Mat(roiMask));
            viewer->notify();
        }

        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        {
            std::lock_guard<Libs::Spinlock> lock(slock_processing_times);
            processing_times.push_back(elapsed.count());
        }
    }
}

void MotionDetector::notify() {
    cond.notify();
}

bool MotionDetector::operator==(const MotionDetector & other) const
{
    return &other == this;
}

std::string MotionDetector::watchdog() {
    std::string result = "";
    std::lock_guard<Libs::Spinlock> lock(slock_processing_times);
    auto size = processing_times.size();
    if(size > fps) {    // > ~ 1s
        int64_t sum = 0;
        for(auto & time : processing_times) {
            sum += time;
        }
        double max = *std::max_element(processing_times.begin(), processing_times.end());
        double ratio = max / (double)ms_one_frame;
        double ratioround = std::ceil(ratio);
        /*int64_t tmp = frame_skipping;
        if(max > ms_one_frame) {
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
        // result = std::format("WATCHDOG : MotionDetector : Frame skipping at {}, frame ms at {}, max = {}, ratio = {} ratioround = {}", frame_skipping, ms_one_frame, max, ratio, ratioround);
        result = fmt::format("WATCHDOG : MotionDetector : max = {}", max);
    }
    return result;
}

void MotionDetector::reloadConfig(Libs::Config & config) {
    cv_motion_history = config.get<int>("opencv_model_history");
    cv_motion_var_threshold = config.get<int>("opencv_model_var_threshold");
    cv_motion_detect_shadows = config.get<bool>("opencv_model_detect_shadows");

    stop();

    start();

    Libs::Logger::log_main("MotionDetector::reloadConfig : MotionDetector reloaded");
}
