
#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <cstdint>

#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Libs/Stream.h"


namespace DelNico::RtspAnalyser::Streamers {
  class StreamReceiver {
    public:
        StreamReceiver() = delete;
        StreamReceiver(
            boost::asio::io_service & io_service,
            const Libs::Stream & stream,
            std::deque<cv::Mat> & frames
        );
        ~StreamReceiver();
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


