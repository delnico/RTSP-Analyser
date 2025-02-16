#include <thread>
#include <atomic>
#include <vector>
#include <deque>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/OutputStream.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"

#include "Nico/RtspAnalyser/Analyser/Multiplexer.h"

using namespace Nico::RtspAnalyser::Analyser;

Multiplexer::Multiplexer(
    std::deque<cv::Mat> & input_frames
) :
    isEnabled(false),
    thread(),
    input_cond(),
    input_frames(input_frames),
    output_clients(),
    frame_count(0),
    is_stream_redirecting(false),
    stream_redirect_client(nullptr)
{}


Multiplexer::~Multiplexer() {
    stop();
}

void Multiplexer::subscribe(OutputStream * output_client) {
    output_clients.push_back(output_client);
}

void Multiplexer::unsubscribe(OutputStream * output_client) {
    output_clients.remove(output_client);
}

void Multiplexer::start() {
    if(! isEnabled.load()) {
        isEnabled.store(true);
        thread = std::thread(&Multiplexer::run, this);
    }
}

void Multiplexer::stop() {
    if(isEnabled.load()) {
        isEnabled.store(false);
        input_cond.notify();
        thread.join();
    }
}

void Multiplexer::notify() {
    input_cond.notify();
}

void Multiplexer::start_stream_redirect_human_detector() {
    is_stream_redirecting.store(true);
}

void Multiplexer::stop_stream_redirect_human_detector() {
    is_stream_redirecting.store(false);
}

void Multiplexer::set_stream_redirect_client(OutputStream * stream_redirect_client) {
    stream_redirect_client = stream_redirect_client;
}

void Multiplexer::run() {
    cv::Mat frame;
    while(isEnabled.load()) {
        input_cond.wait();

        frame_count++;
        
        if(! input_frames.empty())
        {
            frame = input_frames.front();
            input_frames.pop_front();

            multiplex(frame);
        }
    }
}

void Multiplexer::multiplex(cv::Mat frame) {
    for(OutputStream * oc : output_clients) {
        int64_t frame_skipping = oc->getFrameSkipping();
        if(frame_skipping > 1) {
            if(frame_count % frame_skipping != 0)
                continue;
        }
        oc->addFrame(frame);
        oc->notify();
    }
    if(stream_redirect_client != nullptr && is_stream_redirecting.load()) {
        int64_t frame_skipping = stream_redirect_client->getFrameSkipping();
        if(frame_skipping > 1) {
            if(frame_count % frame_skipping != 0)
                return;
        }
        stream_redirect_client->addFrame(frame);
        stream_redirect_client->notify();
    }
}
