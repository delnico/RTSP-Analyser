#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <string>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Analyser {
            class Viewer : public IAnalyser {
                public:
                    Viewer();
                    // windowName is can't be empty, can't be a space, can't be a tabulation, ...
                    Viewer(std::deque<cv::Mat> & frames, std::string windowName = "Viewer");
                    ~Viewer();
                    Viewer(const Viewer & other) = delete;
                    Viewer & operator=(const Viewer & other) = delete;
                    void start();
                    void stop();
                    
                private:
                    Nico::RtspAnalyser::Libs::ConditionalVariable cond;
                    std::atomic_flag isEnabled = ATOMIC_FLAG_INIT;
                    std::thread thread;
                    std::deque<cv::Mat> & frames;
                    std::string windowName;

                    void run();

                    void notify() override;
                    void wait() override;

                    bool operator==(const Viewer & other) const;
            };
        }
    }
}
