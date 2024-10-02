
#pragma once

#include <atomic>
#include <string>
#include <deque>
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Libs/Stream.h"
#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Streamer {
          class Streamer {
            public:
                Streamer(const Nico::RtspAnalyser::Libs::Stream & stream);
                ~Streamer();
                void start();
                void stop();
            private:
                std::atomic_flag isEnabled;
                Nico::RtspAnalyser::Libs::Stream stream;
                cv::VideoCapture cap;
                std::deque<cv::Mat> frames;
                std::thread thread;
                std::vector<Nico::RtspAnalyser::Analyser::Analyser> listeners;

                void run();
            };
        }
    }
}
