#pragma once

#include <thread>
#include <atomic>

#include "Nico/RtspAnalyser/Motion/MotionDetector.h"
#include "Nico/RtspAnalyser/Streamers/Streamer.h"
#include "Nico/RtspAnalyser/Libs/Logger.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace WatchdogLib {
            class Watchdog {
                public:
                    Watchdog() = delete;
                    Watchdog(
                        Nico::RtspAnalyser::Streamers::Streamer * streamer,
                        Nico::RtspAnalyser::Motion::MotionDetector * motionDetector,
                        Nico::RtspAnalyser::Libs::Logger * logger
                    );
                    ~Watchdog();

                    void start();
                    void stop();

                private:
                    std::atomic<bool> isEnabled;
                    std::thread thread;

                    Nico::RtspAnalyser::Streamers::Streamer * streamer;
                    Nico::RtspAnalyser::Motion::MotionDetector * motionDetector;
                    Nico::RtspAnalyser::Libs::Logger * logger;

                    void run();
            };
        }
    }
}
