#pragma once

#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"


namespace Nico::RtspAnalyser::Analyser {
    class HumanDetector : public IAnalyser {
        public:
            HumanDetector() = delete;
            HumanDetector(std::deque<cv::Mat> & frames);
            ~HumanDetector() override;
            HumanDetector(const HumanDetector & other) = delete;
            HumanDetector & operator=(const HumanDetector & other) = delete;
            void start();
            void stop();
            void notify() override;

        private:
            Libs::ConditionalVariable cond;
            std::atomic<bool> isEnabled;
            std::thread thread;
            std::deque<cv::Mat> & frames;
            cv::HOGDescriptor hog;

            void run();
            std::tuple<bool, cv::Mat>  isHumanDetected(const cv::Mat & frame, const bool need_output = false) const;

            bool operator==(const HumanDetector & other) const;
    };
}