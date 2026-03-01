#pragma once

#include <atomic>
#include <functional>
#include <vector>
#include <thread>

#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Motion/MotionDetector.h"
#include "DelNico/RtspAnalyser/Streamers/StreamReceiver.h"


namespace DelNico::RtspAnalyser::Watchdog {
    class Watchdog {
        public:
            Watchdog() = delete;
            Watchdog(
                Libs::Logger * logger
            );
            ~Watchdog();

            void start();
            void stop();

            void subscribe(const std::function<void()> & callback);
            void unsubscribe(const std::function<void()> & callback);

        private:
            std::atomic<bool> isEnabled;
            std::thread thread;

            std::vector<std::function<void()>> callbacks;

            Libs::Logger * logger;

            void run();
    };
}


