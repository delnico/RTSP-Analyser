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
            class Viewer : public IAnalyser {
                public:
                    Viewer();
                    Viewer(std::deque<cv::Mat> & frames);
                    ~Viewer();
                    Viewer(const Viewer & other) = delete;
                    Viewer & operator=(const Viewer & other) = delete;
                    void run();
                    void start();
                    void stop();
                    
                private:
                    Nico::RtspAnalyser::Libs::ConditionalVariable cond;
                    std::atomic_flag isEnabled = ATOMIC_FLAG_INIT;
                    std::thread thread;
                    std::deque<cv::Mat> & frames;

                    void notify() override;
                    void wait() override;

                    bool operator==(const Viewer & other) const;
            };
        }
    }
}
