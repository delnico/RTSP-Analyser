#include <thread>
#include <atomic>
#include <chrono>

#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Motion/MotionDetector.h"
#include "DelNico/RtspAnalyser/Streamers/StreamReceiver.h"
#include "DelNico/RtspAnalyser/Watchdog/Watchdog.h"

namespace DelNico::RtspAnalyser::Watchdog {
    Watchdog::Watchdog(
        Libs::Logger * logger
    ) :
        isEnabled(false),
        thread(),
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
            try {
                thread.join();
            }
            catch(const std::exception & e) {
                logger->log(std::string("[Watchdog] Exception while joining thread: ") + e.what());
            }
        }
        else
            logger->log("[Watchdog] thread not joinable");
    }

    void Watchdog::subscribe(const std::function<void()> & callback) {
        callbacks.push_back(callback);
    }
    void Watchdog::unsubscribe(const std::function<void()> & callback) {
        callbacks.erase(
            std::remove_if(
                callbacks.begin(),
                callbacks.end(),
                [&callback](const std::function<void()> & cb) {
                    return cb.target_type() == callback.target_type() &&
                        cb.target<void()>() == callback.target<void()>();
                }
            ),
            callbacks.end()
        );
    }

    void Watchdog::run() {
        while(isEnabled.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            for(const auto & callback : callbacks) {
                try {
                    callback();
                }
                catch(const std::exception & e) {
                    logger->log(std::string("[Watchdog] Exception while running callback: ") + e.what());
                }
            }
        }
    }
}
