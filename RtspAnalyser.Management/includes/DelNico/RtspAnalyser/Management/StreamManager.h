#pragma once

#include <thread>
#include <atomic>

#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"

namespace DelNico::RtspAnalyser::Management {
    class StreamManager {
        public:
            StreamManager();
            ~StreamManager();
            void start();
            void stop();
            void notify();

        private:
            void run();

            std::atomic<bool> isEnabled;
            std::thread thread;
            Libs::ConditionalVariable cond_events;
    };
}