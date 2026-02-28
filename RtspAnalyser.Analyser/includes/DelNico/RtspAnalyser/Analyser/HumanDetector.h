#pragma once

#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/Streamer.h"
#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"


namespace DelNico::RtspAnalyser::Analyser {
    class HumanDetector : public IAnalyser {
        public:
            HumanDetector() = delete;
            HumanDetector(std::deque<cv::Mat> & frames, Motion::MotionManager * motionManager);
            ~HumanDetector() override;
            HumanDetector(const HumanDetector & other) = delete;
            HumanDetector & operator=(const HumanDetector & other) = delete;

            void setStreamer(Streamer * streamer, std::deque<cv::Mat> * human_detected_output);
            
            void start();
            void stop();
            void notify() override;

        private:
            Libs::ConditionalVariable cond;
            std::atomic<bool> isEnabled;
            std::thread thread;
            std::deque<cv::Mat> & frames;
            cv::HOGDescriptor hog;
            Motion::MotionManager * motionManager;
            Streamer * streamer;
            std::deque<cv::Mat> * human_detected_output;

            void run();
            std::tuple<bool, cv::Mat>  isHumanDetected(const cv::Mat & frame, const bool need_output = false) const;

            bool operator==(const HumanDetector & other) const;
    };
}