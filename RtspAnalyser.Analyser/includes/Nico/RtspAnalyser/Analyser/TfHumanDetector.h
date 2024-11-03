#pragma once

#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Analyser {
            class TfHumanDetector : public IAnalyser {
                public:
                    TfHumanDetector() = delete;
                    TfHumanDetector(
                        std::deque<cv::Mat> & input_frames
                    );
                    ~TfHumanDetector();

                    void start();
                    void stop();

                    void notify() override;

                private:
                    void run();
                    void detect(cv::Mat frame);

                    std::atomic<bool> isEnabled;
                    std::thread thread;
                    std::deque<cv::Mat> & input_frames;
                    Nico::RtspAnalyser::Libs::ConditionalVariable input_cond;
            };
        }
    }
}