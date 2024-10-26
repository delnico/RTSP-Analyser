#include <atomic>
#include <string>
#include <deque>
#include <thread>
#include <vector>
#include <memory>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Streamer/Streamer.h"
#include "Nico/RtspAnalyser/Libs/Stream.h"
#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"

using namespace Nico::RtspAnalyser::Streamers;


Streamer::Streamer(const Nico::RtspAnalyser::Libs::Stream & stream, std::deque<cv::Mat> & frames) :
    isEnabled(ATOMIC_FLAG_INIT),
    stream(stream),
    cap(),
    frames(frames),
    thread()
{
}

Streamer::~Streamer()
{
    stop();
}

void Streamer::start()
{
    isEnabled.store(true);
    cap.open(stream.url, cv::CAP_FFMPEG);
    if(!cap.isOpened())
        throw std::runtime_error("Failed to open stream");
    thread = std::thread(&Streamer::run, this);
}

void Streamer::stop()
{
    if(thread.joinable())
    {
        isEnabled.store(false);
        thread.join();
    }
}

void Streamer::subscribe(Nico::RtspAnalyser::Analyser::IAnalyser * analyser)
{
    listener = analyser;
}

void Streamer::unsubscribe(Nico::RtspAnalyser::Analyser::IAnalyser * analyser)
{
    listener = nullptr;
}

int64_t Streamer::queueSize() const
{
    return frames.size();
}

void Streamer::goToLive()
{
    // let clear frames queue but keep the last frame (and one before, for secure queue)
    while(frames.size() > 2)
    {
        frames.pop_front();
    }
}

void Streamer::run()
{
    cv::Mat frame;
    while (isEnabled.load())
    {
        auto start = std::chrono::high_resolution_clock::now();
        cap >> frame;

        if(! frame.empty() && frame.size().width > 0 && frame.size().height > 0)
        {
            if(listener != nullptr)
            {
                frames.push_back(frame);
                listener->notify();
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/30) - elapsed);
    }
}
