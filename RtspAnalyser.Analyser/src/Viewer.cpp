#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/Viewer.h"

using namespace DelNico::RtspAnalyser::Analyser;

Viewer::Viewer(std::deque<cv::Mat> & frames, std::string windowName) :
    cond(),
    isEnabled(false),
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
    isEnabled.store(true);
    thread = std::thread(&Viewer::run, this);
}

void Viewer::stop()
{
    if(thread.joinable())
    {
        isEnabled.store(false);
        notify();
        thread.join();
    }
}

void Viewer::run()
{
    namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::Mat frame;
    while (isEnabled)
    {
        cond.wait();
        if(frames.empty())
            continue;
        frame = frames.front();
        frames.pop_front();
        imshow(windowName, frame);
        cv::waitKey(1);                 // No pause, scheduled by Streamer thread
    }
    cv::destroyWindow(windowName);
}

void Viewer::notify()
{
    cond.notify();
}


bool Viewer::operator==(const Viewer & other) const
{
    return &other == this;
}

