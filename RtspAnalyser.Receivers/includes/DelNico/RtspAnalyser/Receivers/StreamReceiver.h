
#pragma once

#include <atomic>
#include <memory>
#include <cstdint>

#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <oneapi/tbb/concurrent_queue.h>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Libs/Stream.h"


namespace DelNico::RtspAnalyser::Receivers {
  class StreamReceiver {
    public:
        StreamReceiver() = delete;
        StreamReceiver(
            boost::asio::io_context & io_service,
            const Libs::Stream & stream,
            oneapi::tbb::concurrent_queue<cv::Mat> & frames
        );
        ~StreamReceiver();
        void start(
            boost::asio::io_context & io_service,
            std::string nvr_ip,
            int nvr_port,
            std::string nvr_user,
            std::string nvr_password,
            std::string stream_path,
            std::string gstreamer_pipeline_params
        );
        void stop();
        void subscribe(Analyser::IAnalyser * analyser);
        void unsubscribe(Analyser::IAnalyser * analyser);

        void watchdog();

    private:
        int64_t queueSize() const;
        void goToLive();
        void run();
        void autoReloadAfterCrash();

        std::atomic<bool> isEnabled;
        boost::asio::steady_timer timer;
        Libs::Stream stream;
        cv::VideoCapture cap;
        oneapi::tbb::concurrent_queue<cv::Mat> & frames;

        Analyser::IAnalyser * listener;

        std::string gstreamer_pipeline;
    };
}


