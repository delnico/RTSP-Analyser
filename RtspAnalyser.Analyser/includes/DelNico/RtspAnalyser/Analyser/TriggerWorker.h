#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include "DelNico/RtspAnalyser/Libs/Spinlock.h"
#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"

namespace DelNico::RtspAnalyser::Motion {
    class TriggerWorker {
        public:
            TriggerWorker() = delete;
            TriggerWorker(
                const std::string & server_url,
                const std::string & username,
                const std::string & password
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

            std::deque<MotionEvent> events;
            const std::string & server_url;
            const std::string & username;
            const std::string & password;
    };
}
