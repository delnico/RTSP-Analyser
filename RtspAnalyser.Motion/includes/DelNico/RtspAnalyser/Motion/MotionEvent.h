#pragma once

#include <cstdint>
#include <chrono>

#include <opencv2/opencv.hpp>


namespace DelNico::RtspAnalyser::Motion {
    class MotionEvent {
        public:
            MotionEvent();
            MotionEvent(int stream_id);
            ~MotionEvent();

            void update();
            void setMotionDetected(bool status);
            void setHumanDetected(bool status);
            bool isMotionDetected() const;
            bool isHumanDetected() const;
            bool isAlreadyBeenTriggered() const;
            void setAlreadyBeenTriggered();

            bool isMotionTimeCloseTo(int64_t guard_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(5)).count()) const;

            int getStreamId() const;

            int64_t getStartTimestamp() const;
            int64_t getEndTimestamp() const;

            void setPreviewImage(const cv::Mat & frame);
            std::string getPreviewImage() const;
            void setScore(float score);
            float getScore() const;
            
        private:
            int stream_id;
            int64_t start_timestamp;
            int64_t end_timestamp;
            bool motionDetected;
            bool humanDetected;
            bool haveBeenTriggered;

            std::string preview_image_b64;
            float score;
    };
}

