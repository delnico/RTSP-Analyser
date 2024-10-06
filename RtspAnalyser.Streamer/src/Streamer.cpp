#include <atomic>
#include <string>
#include <deque>
#include <thread>
#include <vector>
#include <memory>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Streamer/Streamer.h"
#include "Nico/RtspAnalyser/Libs/Stream.h"
#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"

using namespace Nico::RtspAnalyser::Streamers;


// Streamer::Streamer() :
//     isEnabled(ATOMIC_FLAG_INIT),
//     stream(),
//     cap(),
//     frames(std::deque<cv::Mat>()),
//     thread()
// {
// }

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
    isEnabled.test_and_set(std::memory_order_release);
    cap.open(stream.url, cv::CAP_FFMPEG);
    if(!cap.isOpened())
        throw std::runtime_error("Failed to open stream");
    thread = std::thread(&Streamer::run, this);
}

void Streamer::stop()
{
    if(thread.joinable())
    {
        isEnabled.clear(std::memory_order_release);
        thread.join();
    }
}

void Streamer::subscribe(Nico::RtspAnalyser::Analyser::IAnalyser * analyser)
{
    listeners.push_back(analyser);
}

void Streamer::unsubscribe(Nico::RtspAnalyser::Analyser::IAnalyser * analyser)
{
    listeners.erase(std::remove(listeners.begin(), listeners.end(), analyser), listeners.end());
}

void Streamer::run()
{
    cv::Mat frame;
    while (isEnabled.test_and_set(std::memory_order_acquire))
    {
        auto start = std::chrono::high_resolution_clock::now();
        cap >> frame;

        if(listeners.size() > 0)
        {
            frames.push_back(frame);
            for(auto listener : listeners)
            {
                listener->notify();
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/30) - elapsed);
    }
}
