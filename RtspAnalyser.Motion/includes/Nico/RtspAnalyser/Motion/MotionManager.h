#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include <boost/asio.hpp>

#include "Nico/RtspAnalyser/Analyser/Multiplexer.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Motion/MotionEvent.h"



namespace Nico::RtspAnalyser::Motion {
    class MotionManager {
    public:
        MotionManager() = delete;
        MotionManager(
            boost::asio::io_service & boost_io_service,
            Libs::ConditionalVariable & cond_events,
            std::chrono::seconds guard_time_new_event,
            Analyser::Multiplexer * multiplexer
        );
        ~MotionManager();

        void start();
        void stop();

        void notify();

    private:
        void run();

        void stop_stream_redirect_tensorflow();

        boost::asio::io_service & boost_io_service;
        boost::asio::deadline_timer timer_stream_redirect_tensorflow;
        std::thread thread;
        std::atomic<bool> isEnabled;
        std::deque<MotionEvent> events;
        Libs::ConditionalVariable & cond_events;
        std::chrono::seconds guard_time_new_event;
        Analyser::Multiplexer * multiplexer;
    };
}

