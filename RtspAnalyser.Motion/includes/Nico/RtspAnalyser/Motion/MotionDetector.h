#pragma once

#include <thread>
#include <list>
#include <atomic>
#include <deque>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Analyser {
            class MotionDetector : public IAnalyser {
                public:
                    MotionDetector() = delete;
                    MotionDetector(std::deque<cv::Mat> & frames);
                    ~MotionDetector();

                    void start();
                    void stop();

                private:
                    Nico::RtspAnalyser::Libs::ConditionalVariable cond;
                    std::atomic_flag isEnabled;
                    std::thread thread;
                    std::list<cv::Rect> zones;
                    std::deque<cv::Mat> & frames;
                    std::deque<cv::Mat> fgMasks;
                    Nico::RtspAnalyser::Analyser::Viewer * viewer;
                    int cv_motion_history;
                    double cv_motion_var_threshold;
                    bool cv_motion_detect_shadows;

                    void run();

                    void notify() override;
                    void wait() override;

                    bool operator==(const MotionDetector & other) const;
            };
        }
    }
}
