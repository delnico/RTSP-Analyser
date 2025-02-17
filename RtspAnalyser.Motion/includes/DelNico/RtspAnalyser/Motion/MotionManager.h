#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include <boost/asio.hpp>

#include "DelNico/RtspAnalyser/Analyser/Multiplexer.h"
#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Libs/Spinlock.h"
#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCalling.h"



namespace DelNico::RtspAnalyser::Motion {
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

        void notify(MotionManagerCalling motionManagerCalling);

    private:
        void run();
        void run_called_by_motion_detector();
        void run_called_by_human_detector();

        void stop_stream_redirect_human_detector();

        boost::asio::io_service & boost_io_service;
        boost::asio::deadline_timer timer_stream_redirect_human_detection;
        std::thread thread;
        std::atomic<bool> isEnabled;
        std::deque<MotionEvent> events;
        Libs::ConditionalVariable & cond_events;
        std::chrono::seconds guard_time_new_event;
        Analyser::Multiplexer * multiplexer;
        std::deque<MotionManagerCalling> motionManagerCallings;
        Libs::Spinlock slock_motionManagerCallings;
    };
}

