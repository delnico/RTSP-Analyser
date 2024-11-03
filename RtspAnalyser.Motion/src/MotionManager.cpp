#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "Nico/RtspAnalyser/Analyser/Multiplexer.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Motion/MotionEvent.h"
#include "Nico/RtspAnalyser/Motion/MotionManager.h"

using namespace Nico::RtspAnalyser::Motion;

MotionManager::MotionManager(
    boost::asio::io_service & boost_io_service,
    Nico::RtspAnalyser::Libs::ConditionalVariable & cond_events,
    std::chrono::seconds guard_time_new_event,
    Nico::RtspAnalyser::Analyser::Multiplexer * multiplexer
) :
    boost_io_service(boost_io_service),
    timer_stream_redirect_tensorflow(
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

        timer_stream_redirect_tensorflow.cancel();
        timer_stream_redirect_tensorflow = boost::asio::deadline_timer(
            boost_io_service,
            boost::posix_time::microsec(
                std::chrono::duration_cast<std::chrono::microseconds>(guard_time_new_event).count()
            )
        );
        timer_stream_redirect_tensorflow.async_wait(
            boost::bind(&MotionManager::stop_stream_redirect_tensorflow, this)
        );

        multiplexer->start_stream_redirect_tensorflow();
    }
}

void MotionManager::stop_stream_redirect_tensorflow() {
    timer_stream_redirect_tensorflow.cancel();
    multiplexer->stop_stream_redirect_tensorflow();
}
