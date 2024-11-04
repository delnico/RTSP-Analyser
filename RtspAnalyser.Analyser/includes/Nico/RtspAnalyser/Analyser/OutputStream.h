#pragma once

#include <deque>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"



namespace Nico::RtspAnalyser::Analyser {
    class OutputStream {
        public:
            OutputStream() = delete;
            OutputStream(
                IAnalyser * output,
                std::deque<cv::Mat> & frames,
                int64_t frame_skipping
            );
            ~OutputStream() = default;

            void notify();
            void addFrame(cv::Mat frame);
            cv::Mat getFrame();
            int64_t getFrameSkipping();

        private:
            Nico::RtspAnalyser::Analyser::IAnalyser * output;
            std::deque<cv::Mat> & frames;
            int64_t frame_skipping;
    };
}

