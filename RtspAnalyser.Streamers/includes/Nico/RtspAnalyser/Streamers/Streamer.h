
#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <cstdint>

#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>

#include "Nico/RtspAnalyser/Libs/Stream.h"
#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"



namespace Nico::RtspAnalyser::Streamers {
  class Streamer {
    public:
        Streamer() = delete;
        Streamer(
            boost::asio::io_service & io_service,
            const Libs::Stream & stream,
            std::deque<cv::Mat> & frames
        );
        ~Streamer();
        void start(boost::asio::io_service & io_service);
        void stop();
        void subscribe(Analyser::IAnalyser * analyser);
        void unsubscribe(Analyser::IAnalyser * analyser);

        void watchdog();

    private:
        int64_t queueSize() const;
        void goToLive();
        void run();

        std::atomic<bool> isEnabled;
        boost::asio::deadline_timer timer;
        Libs::Stream stream;
        cv::VideoCapture cap;
        std::deque<cv::Mat> & frames;

        Analyser::IAnalyser * listener;
    };
}


