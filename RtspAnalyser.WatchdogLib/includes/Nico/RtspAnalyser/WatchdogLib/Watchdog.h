#pragma once

#include <thread>
#include <atomic>

#include "Nico/RtspAnalyser/Motion/MotionDetector.h"
#include "Nico/RtspAnalyser/Streamers/Streamer.h"
#include "Nico/RtspAnalyser/Libs/Logger.h"



namespace Nico::RtspAnalyser::WatchdogLib {
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


