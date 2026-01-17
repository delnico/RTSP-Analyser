#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <string>

#include <opencv2/opencv.hpp>
#include <zmq.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"



namespace DelNico::RtspAnalyser::Analyser {
    class Viewer : public IAnalyser {
        public:
            Viewer();
            Viewer(std::deque<cv::Mat> & frames, std::string socket_bind, zmq::context_t & zmqContext);
            ~Viewer() override;
            Viewer(const Viewer & other) = delete;
            Viewer & operator=(const Viewer & other) = delete;
            void start();
            void stop();
            void notify() override;

        private:
            Libs::ConditionalVariable cond;
            std::atomic<bool> isEnabled;
            std::thread thread;
            std::deque<cv::Mat> & frames;
            zmq::socket_t socket;

            void run();

            bool operator==(const Viewer & other) const;
    };
}


