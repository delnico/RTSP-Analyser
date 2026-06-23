#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Libs/Spinlock.h"
#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"

namespace DelNico::RtspAnalyser::Analyser {
    class TriggerWorker {
        public:
            TriggerWorker() = delete;
            TriggerWorker(
                std::string server_url,
                int server_port,
                std::string username
            );
            ~TriggerWorker();

            void start();
            void stop();

            void addEvent(const Motion::MotionEvent & event);
        
        private:
            void run();

            std::thread thread;
            std::atomic<bool> isEnabled;
            Libs::Spinlock slock_events;
            Libs::ConditionalVariable cv_wakeup;


            std::deque<Motion::MotionEvent> events;
            std::string server_url;
            int server_port;
            std::string username;
    };
}
