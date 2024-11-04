#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <string>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"



namespace Nico::RtspAnalyser::Analyser {
    class Viewer : public IAnalyser {
        public:
            Viewer();
            // windowName is can't be empty, can't be with a space, can't be with a tabulation, ...
            Viewer(std::deque<cv::Mat> & frames, std::string windowName = "Viewer");
            ~Viewer();
            Viewer(const Viewer & other) = delete;
            Viewer & operator=(const Viewer & other) = delete;
            void start();
            void stop();
            void notify() override;

        private:
            Nico::RtspAnalyser::Libs::ConditionalVariable cond;
            std::atomic<bool> isEnabled;
            std::thread thread;
            std::deque<cv::Mat> & frames;
            std::string windowName;

            void run();

            bool operator==(const Viewer & other) const;
    };
}


