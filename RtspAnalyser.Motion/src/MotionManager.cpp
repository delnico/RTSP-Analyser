#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "DelNico/RtspAnalyser/Analyser/Multiplexer.h"
#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"

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
}

void MotionManager::stop_stream_redirect_human_detector() {
    timer_stream_redirect_human_detection.cancel();
    multiplexer->stop_stream_redirect_human_detector();
}
