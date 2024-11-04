#include <cstdint>
#include <chrono>

#include "Nico/RtspAnalyser/Motion/MotionEvent.h"

using namespace Nico::RtspAnalyser::Motion;

MotionEvent::MotionEvent() :
    start_timestamp(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
    end_timestamp(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
    motionDetected(false),
    humanDetected(false)
{
    auto start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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

bool MotionEvent::isMotionTimeCloseTo(int64_t guard_time) const {
    auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return now - end_timestamp < guard_time;
}
