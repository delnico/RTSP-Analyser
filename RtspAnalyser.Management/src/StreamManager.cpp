#include <thread>
#include <atomic>

#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Management/StreamManager.h"

using namespace DelNico::RtspAnalyser::Management;

StreamManager::StreamManager() : isEnabled(false) {}

StreamManager::~StreamManager() {}

void StreamManager::start() {
    if(! isEnabled.load()) {
        isEnabled.store(true);
        thread = std::thread(&StreamManager::run, this);
    }
}

void StreamManager::stop() {
    if(isEnabled.load()) {
        isEnabled.store(false);
        notify();
        thread.join();
    }
}

void StreamManager::notify() {
    cond_events.notify();
}

void StreamManager::run() {
    while(isEnabled.load()) {
        cond_events.wait();
    }
}