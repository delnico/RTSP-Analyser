
#pragma once

#include <atomic>
#include <string>
#include <deque>
#include <thread>
#include <vector>
#include <memory>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Libs/Stream.h"
#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Streamers {
          class Streamer {
            public:
                Streamer() = delete;
                Streamer(const Nico::RtspAnalyser::Libs::Stream & stream, std::deque<cv::Mat> & frames);
                ~Streamer();
                void start();
                void stop();
                void subscribe(Nico::RtspAnalyser::Analyser::IAnalyser * analyser);
                void unsubscribe(Nico::RtspAnalyser::Analyser::IAnalyser * analyser);

                int64_t queueSize() const;
                void goToLive();

            private:
                std::atomic<bool> isEnabled;
                Nico::RtspAnalyser::Libs::Stream stream;
                cv::VideoCapture cap;
                std::deque<cv::Mat> & frames;
                std::thread thread;

                Nico::RtspAnalyser::Analyser::IAnalyser * listener;

                void run();
            };
        }
    }
}
