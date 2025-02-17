#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "DelNico/RtspAnalyser/Analyser/Multiplexer.h"
#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Libs/Spinlock.h"
#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCaller.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCalling.h"

using namespace DelNico::RtspAnalyser::Motion;

MotionManager::MotionManager(
    boost::asio::io_service & boost_io_service,
    Libs::ConditionalVariable & cond_events,
    std::chrono::seconds guard_time_new_event,
    Analyser::Multiplexer * multiplexer
) :
    boost_io_service(boost_io_service),
    timer_stream_redirect_human_detection(
        boost_io_service,
        boost::posix_time::microsec(
            std::chrono::duration_cast<std::chrono::microseconds>(guard_time_new_event).count()
        )
    ),
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
        notify(MotionManagerCalling());
        thread.join();
    }
}

void MotionManager::notify(MotionManagerCalling motionManagerCalling) {
    std::lock_guard<Libs::Spinlock> lock(slock_motionManagerCallings);
    motionManagerCallings.push_back(motionManagerCalling);
    cond_events.notify();
}

void MotionManager::run() {
    MotionManagerCalling motionManagerCalling;
    while(isEnabled.load()) {
        cond_events.wait();
        {
            std::lock_guard<Libs::Spinlock> lock(slock_motionManagerCallings);
            if(motionManagerCallings.empty()) {
                continue;
            }
            motionManagerCalling = motionManagerCallings.front();
            motionManagerCallings.pop_front();
        }
        switch(motionManagerCalling.getWho()) {
            case MotionManagerCaller::MOTION_DETECTOR:
                run_called_by_motion_detector();
                break;
            case MotionManagerCaller::HUMAN_DETECTOR:
                run_called_by_human_detector();
                break;
            default:
                throw std::runtime_error("MotionManager::run : unknown motionManagerCalling");
                break;
        }
    }
}

void MotionManager::run_called_by_motion_detector() {
    timer_stream_redirect_human_detection.cancel();
    timer_stream_redirect_human_detection = boost::asio::deadline_timer(
        boost_io_service,
        boost::posix_time::microsec(
            std::chrono::duration_cast<std::chrono::microseconds>(guard_time_new_event).count()
        )
    );
    timer_stream_redirect_human_detection.async_wait(
        boost::bind(&MotionManager::stop_stream_redirect_human_detector, this)
    );

    multiplexer->start_stream_redirect_human_detector();
}

void MotionManager::run_called_by_human_detector() {
    // Do something
}

void MotionManager::stop_stream_redirect_human_detector() {
    timer_stream_redirect_human_detection.cancel();
    multiplexer->stop_stream_redirect_human_detector();
}
