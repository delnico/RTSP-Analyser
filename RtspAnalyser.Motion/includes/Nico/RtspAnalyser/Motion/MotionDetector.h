#pragma once

#include <thread>
#include <list>
#include <atomic>
#include <deque>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Libs/Config.h"
#include "Nico/RtspAnalyser/Libs/Spinlock.h"

#include "Nico/RtspAnalyser/Motion/MotionManager.h"



namespace Nico::RtspAnalyser::Motion {
    class MotionDetector : public Analyser::IAnalyser {
        public:
            MotionDetector() = delete;
            MotionDetector(
                Libs::Config & config,
                std::deque<cv::Mat> & frames,
                std::deque<cv::Mat> & fgMasks,
                int64_t fps
            );
            ~MotionDetector() override;

            void start();
            void stop();
            void setViewer(Analyser::Viewer * viewer);
            void setMotionManager(MotionManager * motionManager);

            std::string watchdog();

            void reloadConfig(Libs::Config & config);

        private:
            Libs::ConditionalVariable cond;
            std::atomic<bool> isEnabled;
            std::thread thread;
            std::list<cv::Rect> zones;
            std::deque<cv::Mat> & frames;
            std::deque<cv::Mat> & fgMasks;
            Analyser::Viewer * viewer;
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


