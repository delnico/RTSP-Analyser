#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"

using namespace Nico::RtspAnalyser::Analyser;

Viewer::Viewer(std::deque<cv::Mat> & frames, std::string windowName) :
    cond(),
    isEnabled(ATOMIC_FLAG_INIT),
    thread(),
    windowName(windowName),
    frames(frames)
{
}

Viewer::~Viewer()
{
    stop();
}

void Viewer::start()
{
    if(! isEnabled.test_and_set(std::memory_order_acquire))
        thread = std::thread(&Viewer::run, this);
}

void Viewer::stop()
{
    if(thread.joinable())
    {
        isEnabled.clear(std::memory_order_release);
        notify();
        thread.join();
    }
}

void Viewer::run()
{
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::Mat frame;
    while (isEnabled.test())
    {
        wait();
        if(frames.empty())
            continue;
        frame = frames.front();
        frames.pop_front();
        cv::imshow(windowName, frame);
        cv::waitKey(1);                 // No pause, scheduled by Streamer thread
    }
    cv::destroyWindow(windowName);
}

void Viewer::notify()
{
    cond.notify();
}

void Viewer::wait()
{
    cond.wait();
}


bool Viewer::operator==(const Viewer & other) const
{
    return &other == this;
}

