#pragma once

#include <thread>
#include <atomic>

#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Motion/MotionDetector.h"
#include "DelNico/RtspAnalyser/Streamers/StreamReceiver.h"


namespace DelNico::RtspAnalyser::Watchdog {
    class Watchdog {
        public:
            Watchdog() = delete;
            Watchdog(
                Streamers::StreamReceiver * streamer,
                Motion::MotionDetector * motionDetector,
                Libs::Logger * logger
            );
            ~Watchdog();

            void start();
            void stop();

        private:
            std::atomic<bool> isEnabled;
            std::thread thread;

            Streamers::StreamReceiver * streamer;
            Motion::MotionDetector * motionDetector;
            Libs::Logger * logger;

            void run();
    };
}


