#pragma once

#include <thread>
#include <atomic>

#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Motion/MotionDetector.h"
#include "DelNico/RtspAnalyser/Streamers/Streamer.h"


namespace DelNico::RtspAnalyser::WatchdogLib {
    class Watchdog {
        public:
            Watchdog() = delete;
            Watchdog(
                Streamers::Streamer * streamer,
                Motion::MotionDetector * motionDetector,
                Libs::Logger * logger
            );
            ~Watchdog();

            void start();
            void stop();

        private:
            std::atomic<bool> isEnabled;
            std::thread thread;

            Streamers::Streamer * streamer;
            Motion::MotionDetector * motionDetector;
            Libs::Logger * logger;

            void run();
    };
}


