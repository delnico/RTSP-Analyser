#include <atomic>
#include <string>
#include <deque>
#include <thread>
#include <vector>
#include <memory>
#include <cstdint>

#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "DelNico/RtspAnalyser/Streamers/Streamer.h"
#include "DelNico/RtspAnalyser/Libs/Stream.h"
#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"

using namespace DelNico::RtspAnalyser::Streamers;


Streamer::Streamer(
    boost::asio::io_service & io_service,
    const Libs::Stream & stream,
    std::deque<cv::Mat> & frames
) :
    isEnabled(false),
    timer(boost::asio::deadline_timer(io_service, boost::posix_time::microsec(stream.frequency.count()))),
    stream(stream),
    cap(),
    frames(frames)
{
}

Streamer::~Streamer()
{
    stop();
}

void Streamer::start(boost::asio::io_service & io_service)
{
    isEnabled.store(true);
    cap.open(stream.url, cv::CAP_FFMPEG);
    if(!cap.isOpened())
        throw std::runtime_error("Failed to open stream");
    timer = boost::asio::deadline_timer(io_service, boost::posix_time::microsec(stream.frequency.count()));
    timer.async_wait(boost::bind(&Streamer::run, this));
}

void Streamer::stop()
{
    if(isEnabled.load())
    {
        isEnabled.store(false);
        timer.cancel();
    }
}

void Streamer::subscribe(Analyser::IAnalyser * analyser)
{
    listener = analyser;
}

void Streamer::unsubscribe(Analyser::IAnalyser * analyser)
{
    listener = nullptr;
}

void Streamer::watchdog()
{
    if(queueSize() > 3) {
        goToLive();
    }
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
    std::chrono::duration<double> elapsed(0);
    while (isEnabled)
    {
        auto start = std::chrono::steady_clock::now();
        cap >> frame;

        if(! frame.empty() && frame.size().width > 0 && frame.size().height > 0)
        {
            if(listener != nullptr)
            {
                frames.push_back(frame);
                listener->notify();
            }
        }
        auto end = std::chrono::steady_clock::now();
        elapsed = end - start;
        auto sleep_duration = std::chrono::microseconds((int64_t) (stream.frequency.count() - (elapsed.count() * 1000)));
        timer.expires_at(timer.expires_at() + boost::posix_time::microsec(sleep_duration.count()));
        timer.async_wait(boost::bind(&Streamer::run, this));
    }
}
