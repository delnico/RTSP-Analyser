#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <vector>

#include <opencv2/opencv.hpp>

//#include <tensorflow/core/public/session.h>
//#include <tensorflow/core/platform/env.h>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Libs/Config.h"



namespace Nico::RtspAnalyser::Analyser {
    class TfHumanDetector : public IAnalyser {
        public:
            TfHumanDetector() = delete;
            TfHumanDetector(
                std::deque<cv::Mat> & input_frames,
                Libs::Config & config
            );
            ~TfHumanDetector() override;

            void start();
            void stop();

            void notify() override;

        private:
            void run();
            void detect(cv::Mat frame);

            void load_model_labels();

            std::atomic<bool> isEnabled;
            std::thread thread;
            std::deque<cv::Mat> & input_frames;
            Libs::ConditionalVariable input_cond;
            Libs::Config & config;

            std::vector<std::string> model_labels;
    };
}

