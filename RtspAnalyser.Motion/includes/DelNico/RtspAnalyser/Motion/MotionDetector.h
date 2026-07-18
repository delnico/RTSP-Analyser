#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <cstdint>

#include <opencv2/opencv.hpp>
#include <oneapi/tbb/concurrent_queue.h>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/Streamer.h"
#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Libs/Config.h"
#include "DelNico/RtspAnalyser/Libs/Spinlock.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"



namespace DelNico::RtspAnalyser::Motion {
    class MotionDetector : public Analyser::IAnalyser {
        public:
            MotionDetector() = delete;
            MotionDetector(
                Libs::Config & config,
                oneapi::tbb::concurrent_queue<cv::Mat> & frames,
                oneapi::tbb::concurrent_queue<cv::Mat> & fgMasks,
                int64_t fps,
                std::vector<cv::Rect> zones
            );
            ~MotionDetector() override;

            void start();
            void stop();
            void setStreamer(Analyser::Streamer * streamer);
            void setMotionManager(MotionManager * motionManager);

            void watchdog();

            void reloadConfig(Libs::Config & config);

        private:
            Libs::ConditionalVariable cond;
            std::atomic<bool> isEnabled;
            std::thread thread;
            std::vector<cv::Rect> zones;
            oneapi::tbb::concurrent_queue<cv::Mat> & frames;
            oneapi::tbb::concurrent_queue<cv::Mat> & fgMasks;
            Analyser::Streamer * streamer;
            MotionManager * motionManager;

            std::atomic<int> cv_motion_history;
            std::atomic<double> cv_motion_var_threshold;
            std::atomic<bool> cv_motion_detect_shadows;

            int64_t ms_one_frame;
            int64_t ms_one_frame_original;
            int64_t fps;
            Libs::Spinlock slock_processing_times;
            std::deque<int64_t> processing_times;

            void run();

            void notify() override;

            bool operator==(const MotionDetector & other) const;
    };
}


