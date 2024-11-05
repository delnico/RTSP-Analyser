#include <thread>
#include <atomic>
#include <chrono>

#include "Nico/RtspAnalyser/WatchdogLib/Watchdog.h"
#include "Nico/RtspAnalyser/Motion/MotionDetector.h"
#include "Nico/RtspAnalyser/Streamers/Streamer.h"
#include "Nico/RtspAnalyser/Libs/Logger.h"

using namespace Nico::RtspAnalyser::WatchdogLib;

Watchdog::Watchdog(
    Streamers::Streamer * streamer,
    Motion::MotionDetector * motionDetector,
    Libs::Logger * logger
) :
    isEnabled(false),
    thread(),
    streamer(streamer),
    motionDetector(motionDetector),
    logger(logger)
{}

Watchdog::~Watchdog() {
    this->stop();
}

void Watchdog::start() {
    if(! isEnabled.load()) {
        isEnabled.store(true);
        thread = std::thread(&Watchdog::run, this);
    }
}

void Watchdog::stop() {
    if(thread.joinable()) {
        isEnabled.store(false);
        thread.join();
    }
}

void Watchdog::run() {
    while(isEnabled.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(motionDetector != nullptr) {
            std::string res = motionDetector->watchdog();
            if(res != "") {
                logger->log(res);
            }
        }
        if(streamer != nullptr) {
            streamer->watchdog();
        }
    }
}
