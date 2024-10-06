#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"

using namespace Nico::RtspAnalyser::Analyser;

// Viewer::Viewer() :
//     cond(),
//     isEnabled(ATOMIC_FLAG_INIT),
//     thread(),
//     frames(std::deque<cv::Mat>())
// {
// }

Viewer::Viewer(std::deque<cv::Mat> & frames) :
    cond(),
    isEnabled(ATOMIC_FLAG_INIT),
    thread(),
    frames(frames)
{
}

Viewer::~Viewer()
{
    stop();
}

void Viewer::run()
{
    cv::namedWindow("Viewer", cv::WINDOW_NORMAL);
    cv::Mat frame;
    while (isEnabled.test())
    {
        wait();
        if(frames.empty())
        {
            continue;
        }
        frame = frames.front();
        frames.pop_front();
        cv::imshow("Viewer", frame);
        if(cv::waitKey(33) == 27) break;
    }
    cv::destroyWindow("Viewer");
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

