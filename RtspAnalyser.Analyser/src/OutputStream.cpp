#include <cstdint>

#include <opencv2/opencv.hpp>
#include <oneapi/tbb/concurrent_queue.h>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/OutputStream.h"

using namespace DelNico::RtspAnalyser::Analyser;

OutputStream::OutputStream(
    IAnalyser * output,
    oneapi::tbb::concurrent_queue<cv::Mat> & frames,
    int64_t frame_skipping
) :
    output(output),
    frames(frames),
    frame_skipping(frame_skipping)
{}

void OutputStream::notify() {
    output->notify();
}

void OutputStream::addFrame(cv::Mat frame) {
    frames.push(frame);
}

std::optional<cv::Mat> OutputStream::getFrame() {
    cv::Mat frame;
    if(frames.try_pop(frame)) {
        return frame;
    }
    return std::nullopt;
}

int64_t OutputStream::getFrameSkipping() {
    return frame_skipping;
}
