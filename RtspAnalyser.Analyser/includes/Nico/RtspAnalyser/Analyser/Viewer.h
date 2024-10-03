#pragma once

#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Libs/Spinlock.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Analyser {
            class Viewer : public IAnalyser {
                public:
                    void run();
                    void start();
                    void stop();
                    
                private:
                    Nico::RtspAnalyser::Libs::Spinlock lock;
                    std::atomic_flag isEnabled = ATOMIC_FLAG_INIT;
                    std::thread thread;
                    std::deque<cv::Mat> frames;

                    void notify() override {};
                    void wait() override {};
                    bool operator==(const IAnalyser & other) const override;
            };
        }
    }
}
