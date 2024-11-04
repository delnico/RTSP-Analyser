#pragma once

#include <cstdint>
#include <chrono>



namespace Nico::RtspAnalyser::Motion {
    class MotionEvent {
        public:
            MotionEvent();
            ~MotionEvent();

            void update();
            void setMotionDetected(bool status);
            void setHumanDetected(bool status);
            bool isMotionDetected() const;
            bool isHumanDetected() const;

            bool isMotionTimeCloseTo(int64_t guard_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(5)).count()) const;
        private:
            int64_t start_timestamp;
            int64_t end_timestamp;
            bool motionDetected;
            bool humanDetected;
    };
}

