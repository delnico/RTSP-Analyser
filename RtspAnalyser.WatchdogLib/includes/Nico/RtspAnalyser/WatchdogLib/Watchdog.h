#pragma once

#include <thread>
#include <atomic>

#include "Nico/RtspAnalyser/Motion/MotionDetector.h"
#include "Nico/RtspAnalyser/Streamers/Streamer.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace WatchdogLib {
            class Watchdog {
                public:
                    Watchdog() = delete;
                    Watchdog(
                        Nico::RtspAnalyser::Streamers::Streamer * streamer,
                        Nico::RtspAnalyser::Motion::MotionDetector * motionDetector
                    );
                    ~Watchdog();

                    void start();
                    void stop();

                private:
                    std::atomic<bool> isEnabled;
                    std::thread thread;

                    Nico::RtspAnalyser::Streamers::Streamer * streamer;
                    Nico::RtspAnalyser::Motion::MotionDetector * motionDetector;

                    void run();
            };
        }
    }
}
