#pragma once

#include <thread>
#include <list>
#include <atomic>
#include <deque>
#include <vector>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Libs/Config.h"
#include "Nico/RtspAnalyser/Libs/Spinlock.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Motion {
            class MotionDetector : public Nico::RtspAnalyser::Analyser::IAnalyser {
                public:
                    MotionDetector() = delete;
                    MotionDetector(
                        Nico::RtspAnalyser::Libs::Config & config,
                        std::deque<cv::Mat> & frames,
                        std::deque<cv::Mat> & fgMasks,
                        int64_t fps
                    );
                    ~MotionDetector();

                    void start();
                    void stop();
                    void setViewer(Nico::RtspAnalyser::Analyser::Viewer * viewer);

                    std::string watchdog();

                private:
                    Nico::RtspAnalyser::Libs::ConditionalVariable cond;
                    std::atomic<bool> isEnabled;
                    std::thread thread;
                    std::list<cv::Rect> zones;
                    std::deque<cv::Mat> & frames;
                    std::deque<cv::Mat> & fgMasks;
                    Nico::RtspAnalyser::Analyser::Viewer * viewer;
                    int cv_motion_history;
                    double cv_motion_var_threshold;
                    bool cv_motion_detect_shadows;

                    int64_t ms_one_frame;
                    int64_t ms_one_frame_original;
                    int64_t fps;
                    Nico::RtspAnalyser::Libs::Spinlock slock_processing_times;
                    std::deque<int64_t> processing_times;

                    void run();

                    void notify() override;

                    bool operator==(const MotionDetector & other) const;
            };
        }
    }
}
