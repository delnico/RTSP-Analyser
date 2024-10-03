#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"

using namespace Nico::RtspAnalyser::Analyser;

void Viewer::run()
{
    cv::namedWindow("Viewer", cv::WINDOW_NORMAL);
    while (isEnabled.test_and_set(std::memory_order_acquire))
    {
        if(frames.empty())
        {
            continue;
        }
        cv::imshow("Viewer", frames.front());
        frames.pop_front();
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



