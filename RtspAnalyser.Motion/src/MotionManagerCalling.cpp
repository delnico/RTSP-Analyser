#include "DelNico/RtspAnalyser/Motion/MotionManagerCaller.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCalling.h"

using namespace DelNico::RtspAnalyser::Motion;

MotionManagerCalling::MotionManagerCalling() :
    who(MotionManagerCaller::MOTION_DETECTOR),
    human_detected(false)
{}

MotionManagerCalling::MotionManagerCalling(MotionManagerCaller who) :
    who(who),
    human_detected(false)
{}

MotionManagerCalling::MotionManagerCalling(MotionManagerCaller who, bool human_detected) :
    who(who),
    human_detected(human_detected)
{}

MotionManagerCalling::~MotionManagerCalling() {}

MotionManagerCaller MotionManagerCalling::getWho() const {
    return who;
}

bool MotionManagerCalling::isHumanDetected() const {
    return human_detected;
}

const bool MotionManagerCalling::operator==(const MotionManagerCalling & other) const {
    return this == &other;
}
