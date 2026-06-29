#include "DelNico/RtspAnalyser/Motion/MotionManagerCaller.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCalling.h"

namespace DelNico::RtspAnalyser::Motion {

    MotionManagerCalling::MotionManagerCalling() :
        who(MotionManagerCaller::MOTION_DETECTOR),
        human_detected(false),
        score(0.0f)
    {}

    MotionManagerCalling::MotionManagerCalling(MotionManagerCaller who) :
        who(who),
        human_detected(false),
        score(0.0f)
    {}

    MotionManagerCalling::MotionManagerCalling(MotionManagerCaller who, bool human_detected) :
        who(who),
        human_detected(human_detected),
        score(0.0f)
    {}

    MotionManagerCalling::MotionManagerCalling(MotionManagerCaller who, bool human_detected, float score) :
        who(who),
        human_detected(human_detected),
        score(score)
    {}

    MotionManagerCalling::~MotionManagerCalling() {}

    MotionManagerCaller MotionManagerCalling::getWho() const {
        return who;
    }

    bool MotionManagerCalling::isHumanDetected() const {
        return human_detected;
    }

    float MotionManagerCalling::getScore() const {
        return score;
    }

    const bool MotionManagerCalling::operator==(const MotionManagerCalling & other) const {
        return this == &other;
    }

}
