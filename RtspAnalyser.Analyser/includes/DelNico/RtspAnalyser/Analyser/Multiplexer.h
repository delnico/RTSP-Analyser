#pragma once

#include <atomic>
#include <deque>
#include <list>
#include <thread>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/OutputStream.h"
#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"


namespace DelNico::RtspAnalyser::Analyser {
    class Multiplexer : public IAnalyser {
        public:
            Multiplexer() = delete;
            Multiplexer(
                std::deque<cv::Mat> & input_frames
            );
            ~Multiplexer() override;

            void subscribe(OutputStream * output_client);
            void unsubscribe(OutputStream * output_client);

            void start();
            void stop();

            void notify() override;

            void start_stream_redirect_human_detector();
            void stop_stream_redirect_human_detector();
            void set_stream_redirect_client(OutputStream * stream_redirect_client);

        private:
            void run();
            void multiplex(cv::Mat frame);

            std::atomic<bool> isEnabled;
            std::thread thread;
            Libs::ConditionalVariable input_cond;

            std::deque<cv::Mat> & input_frames;

            std::list<OutputStream *> output_clients;
            int64_t frame_count;

            std::atomic<bool> is_stream_redirecting;
            OutputStream * stream_redirect_client;
    };
}


