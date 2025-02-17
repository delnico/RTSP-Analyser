#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/HumanDetector.h"
#include "DelNico/RtspAnalyser/Libs/Logger.h"

using namespace DelNico::RtspAnalyser::Analyser;
using namespace DelNico::RtspAnalyser::Libs;

HumanDetector::HumanDetector(std::deque<cv::Mat> & frames) :
    cond(),
    isEnabled(false),
    thread(),
    frames(frames),
    hog()
{
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

HumanDetector::~HumanDetector()
{
    stop();
}

void HumanDetector::start()
{
    isEnabled.store(true);
    thread = std::thread(&HumanDetector::run, this);
}

void HumanDetector::stop()
{
    if(thread.joinable())
    {
        isEnabled.store(false);
        notify();
        thread.join();
    }
}

void HumanDetector::run()
{
    cv::Mat frame;
    std::tuple<bool, cv::Mat> result;
    while (isEnabled)
    {
        cond.wait();
        if(frames.empty())
            continue;
        frame = frames.front();
        frames.pop_front();
        result = isHumanDetected(frame, true);
        if(std::get<0>(result))
        {
            Logger::log_main("HumanDetector : detected");
            // Do something
        }
    }
}

std::tuple<bool, cv::Mat> HumanDetector::isHumanDetected(const cv::Mat & frame, const bool need_output) const
{
    cv::Mat gray, output;
    output = frame.clone();
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    auto boxes = std::vector<cv::Rect>();
    auto weights = std::vector<double>();

    hog.detectMultiScale(gray, boxes, weights);

    if (boxes.size() > 0)
    {
        if(need_output)
        {
            for (size_t i = 0; i < boxes.size(); i++)
            {
                cv::rectangle(output, boxes[i], cv::Scalar(0, 255, 0), 2);
            }
        }
        return std::make_tuple(true, output);
    }

    return std::make_tuple(false, output);
}

void HumanDetector::notify()
{
    cond.notify();
}

bool HumanDetector::operator==(const HumanDetector & other) const
{
    return this == &other;
}
