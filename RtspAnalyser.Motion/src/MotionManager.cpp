#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include "Nico/RtspAnalyser/Analyser/Multiplexer.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Motion/MotionEvent.h"
#include "Nico/RtspAnalyser/Motion/MotionManager.h"

using namespace Nico::RtspAnalyser::Motion;

MotionManager::MotionManager(
    Nico::RtspAnalyser::Libs::ConditionalVariable & cond_events,
    std::chrono::seconds guard_time_new_event,
    Nico::RtspAnalyser::Analyser::Multiplexer * multiplexer
) :
    thread(),
    isEnabled(false),
    events(),
    cond_events(cond_events),
    guard_time_new_event(guard_time_new_event),
    multiplexer(multiplexer)
{}

MotionManager::~MotionManager() {}

void MotionManager::start() {
    if(! isEnabled.load()) {
        isEnabled.store(true);
        thread = std::thread(&MotionManager::run, this);
    }
}

void MotionManager::stop() {
    if(isEnabled.load()) {
        isEnabled.store(false);
        notify();
        thread.join();
    }
}

void MotionManager::notify() {
    cond_events.notify();
}

void MotionManager::run() {
    while(isEnabled.load()) {
        cond_events.wait();

        // process events
        // ...
    }
}