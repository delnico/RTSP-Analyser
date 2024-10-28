#include <deque>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/OutputStream.h"

using namespace Nico::RtspAnalyser::Analyser;

OutputStream::OutputStream(
    IAnalyser * output,
    std::deque<cv::Mat> & frames,
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
    frames.push_back(frame);
}

cv::Mat OutputStream::getFrame() {
    cv::Mat frame;
    if(! frames.empty()) {
        frame = frames.front();
        frames.pop_front();
    }
    return frame;
}

int64_t OutputStream::getFrameSkipping() {
    return frame_skipping;
}
