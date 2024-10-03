#include <atomic>
#include <string>
#include <deque>
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Streamer/Streamer.h"
#include "Nico/RtspAnalyser/Libs/Stream.h"
#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"

using namespace Nico::RtspAnalyser::Streamer;

Streamer::Streamer() :
    isEnabled(ATOMIC_FLAG_INIT),
    stream(),
    cap(),
    thread()
{
}

Streamer::Streamer(const Nico::RtspAnalyser::Libs::Stream & stream) :
    isEnabled(ATOMIC_FLAG_INIT),
    stream(stream),
    cap(),
    thread(&Streamer::run, this)
{
}

Streamer::~Streamer()
{
    stop();
}

void Streamer::start()
{
    isEnabled.test_and_set(std::memory_order_release);
    thread = std::thread(&Streamer::run, this);
}

void Streamer::stop()
{
    isEnabled.clear(std::memory_order_release);
    thread.join();
}

void Streamer::subscribe(const Nico::RtspAnalyser::Analyser::IAnalyser & analyser)
{
    listeners.push_back(analyser);
}

void Streamer::unsubscribe(const Nico::RtspAnalyser::Analyser::IAnalyser & analyser)
{
    listeners.erase(std::remove(listeners.begin(), listeners.end(), analyser), listeners.end());
}

void Streamer::run()
{
    cap.open(stream.url, cv::CAP_FFMPEG);
    if(!cap.isOpened())
    {
        return;
    }
    while (isEnabled.test_and_set(std::memory_order_acquire))
    {
        auto start = std::chrono::high_resolution_clock::now();
        cv::Mat frame;
        cap >> frame;

        if(listeners.size() > 0)
        {
            frames.push_back(frame);
            for(auto listener : listeners)
            {
                // listener.notify();
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/30) - elapsed);
    }
    cap.release();
}
