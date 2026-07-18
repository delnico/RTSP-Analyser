#pragma once

#include <cstdint>
#include <optional>

#include <opencv2/opencv.hpp>
#include <oneapi/tbb/concurrent_queue.h>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"


namespace DelNico::RtspAnalyser::Analyser {
    class OutputStream {
        public:
            OutputStream() = delete;
            OutputStream(
                IAnalyser * output,
                oneapi::tbb::concurrent_queue<cv::Mat> & frames,
                int64_t frame_skipping
            );
            ~OutputStream() = default;

            void notify();
            void addFrame(cv::Mat frame);
            std::optional<cv::Mat> getFrame();
            int64_t getFrameSkipping();

        private:
            IAnalyser * output;
            oneapi::tbb::concurrent_queue<cv::Mat> & frames;
            int64_t frame_skipping;
    };
}

