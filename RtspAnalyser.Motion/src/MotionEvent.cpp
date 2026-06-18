#include <cstdint>
#include <chrono>

#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"

namespace DelNico::RtspAnalyser::Motion {

    MotionEvent::MotionEvent() :
        start_timestamp(0),
        end_timestamp(0),
        motionDetected(false),
        humanDetected(false),
        haveBeenTriggered(false)
    {
        auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        this->start_timestamp = now;
        this->end_timestamp = now;
    }

    MotionEvent::~MotionEvent() {}

    void MotionEvent::update() {
        end_timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void MotionEvent::setMotionDetected(bool status) {
        motionDetected = status;
    }

    void MotionEvent::setHumanDetected(bool status) {
        humanDetected = status;
    }

    bool MotionEvent::isMotionDetected() const {
        return motionDetected;
    }

    bool MotionEvent::isHumanDetected() const {
        return humanDetected;
    }

    bool MotionEvent::isAlreadyBeenTriggered() const {
        return haveBeenTriggered;
    }

    void MotionEvent::setAlreadyBeenTriggered() {
        haveBeenTriggered = true;
    }

    bool MotionEvent::isMotionTimeCloseTo(int64_t guard_time) const {
        auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return now - end_timestamp < guard_time;
    }

}
