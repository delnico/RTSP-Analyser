#include <cstdint>
#include <chrono>

#include <opencv2/imgcodecs.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"

using namespace boost::archive::iterators;
using base64_enc = base64_from_binary<transform_width<std::vector<uchar>::const_iterator, 6, 8>>;

namespace DelNico::RtspAnalyser::Motion {

    MotionEvent::MotionEvent() : MotionEvent(-1) {}

    MotionEvent::MotionEvent(int stream_id) :
        stream_id(stream_id),
        start_timestamp(0),
        end_timestamp(0),
        motionDetected(false),
        humanDetected(false),
        haveBeenTriggered(false),
        preview_image_b64(""),
        score(0.0f)
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

    int MotionEvent::getStreamId() const {
        return stream_id;
    }

    int64_t MotionEvent::getStartTimestamp() const {
        return start_timestamp;
    }

    int64_t MotionEvent::getEndTimestamp() const {
        return end_timestamp;
    }

    bool MotionEvent::isMotionTimeCloseTo(int64_t guard_time) const {
        auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return now - end_timestamp < guard_time;
    }

    void MotionEvent::setPreviewImage(const cv::Mat & frame) {
        if (frame.empty()) return;

        // compress into jpg
        std::vector<uchar> buf;
        cv::imencode(".jpg", frame, buf, {cv::IMWRITE_JPEG_QUALITY, 80});

        // encode to base64 using Boost
        preview_image_b64 = std::string(base64_enc(buf.begin()), base64_enc(buf.end()));

        size_t padding = (3 - (buf.size() % 3)) % 3;
        preview_image_b64.append(padding, '=');
    }

    std::string MotionEvent::getPreviewImage() const {
        return preview_image_b64;
    }

    void MotionEvent::setScore(float score) {
        this->score = score;
    }
    
    float MotionEvent::getScore() const {
        return score;
    }
}
