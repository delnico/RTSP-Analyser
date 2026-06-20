#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Libs/Spinlock.h"
#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"

namespace DelNico::RtspAnalyser::Motion {
    class TriggerWorker {
        public:
            TriggerWorker() = delete;
            TriggerWorker(
                const std::string & server_url,
                int server_port,
                const std::string & username
            );
            ~TriggerWorker();

            void start();
            void stop();

            void addEvent(const MotionEvent & event);
        
        private:
            void run();

            std::thread thread;
            std::atomic<bool> isEnabled;
            Libs::Spinlock slock_events;
            Libs::ConditionalVariable cv_wakeup;


            std::deque<MotionEvent> events;
            const std::string & server_url;
            int server_port;
            const std::string & username;
    };
}
