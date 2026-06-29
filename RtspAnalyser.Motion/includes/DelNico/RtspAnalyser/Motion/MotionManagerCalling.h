#pragma once

#include "DelNico/RtspAnalyser/Motion/MotionManagerCaller.h"

namespace DelNico::RtspAnalyser::Motion {
    class MotionManagerCalling {
        public:
            MotionManagerCalling();
            MotionManagerCalling(MotionManagerCaller who);
            MotionManagerCalling(MotionManagerCaller who, bool human_detected);
            MotionManagerCalling(MotionManagerCaller who, bool human_detected, int score);
            ~MotionManagerCalling();

            MotionManagerCaller getWho() const;
            bool isHumanDetected() const;
            int getScore() const;
        private:
            MotionManagerCaller who;
            bool human_detected;
            int score;

            const bool operator==(const MotionManagerCalling & other) const;
    };
}