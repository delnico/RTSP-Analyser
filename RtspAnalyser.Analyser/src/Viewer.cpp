#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"

using namespace Nico::RtspAnalyser::Analyser;

Viewer::Viewer()
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
    while (isEnabled.test_and_set(std::memory_order_acquire))
    {
        wait();
        frame = frames.front();
        frames.pop_front();
        if(frame.empty())
        {
            continue;
        }
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
    if(isEnabled.test_and_set(std::memory_order_release))
        thread.join();
}

void Viewer::notify()
{
    lock.unlock();
}

void Viewer::wait()
{
    lock.lock();
}


bool Viewer::operator==(const Viewer & other) const
{
    return &other == this;
}

