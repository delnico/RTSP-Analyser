#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/HumanDetector.h"
#include "DelNico/RtspAnalyser/Analyser/Streamer.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCaller.h"
#include "DelNico/RtspAnalyser/Motion/MotionManagerCalling.h"

using namespace DelNico::RtspAnalyser::Analyser;
using namespace DelNico::RtspAnalyser::Libs;

HumanDetector::HumanDetector(std::deque<cv::Mat> & frames, Motion::MotionManager * motionManager) :
    cond(),
    isEnabled(false),
    thread(),
    frames(frames),
    hog(),
    motionManager(motionManager),
    streamer(nullptr),
    human_detected_output(nullptr)
{
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

HumanDetector::~HumanDetector()
{
    stop();
}

void HumanDetector::setStreamer(Streamer * streamer, std::deque<cv::Mat> * human_detected_output)
{
    this->streamer = streamer;
    this->human_detected_output = human_detected_output;
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

void HumanDetector::notify()
{
    cond.notify();
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
            motionManager->notify(
                Motion::MotionManagerCalling(
                    Motion::MotionManagerCaller::HUMAN_DETECTOR,
                    true
                )
            );
        }
        if(streamer)
        {
            human_detected_output->push_back(std::get<1>(result));
            streamer->notify();
        }
    }
}

std::tuple<bool, cv::Mat> HumanDetector::isHumanDetected(const cv::Mat & frame, const bool need_output) const
{
    cv::Mat gray, output;
    cv::resize(frame, output, cv::Size(frame.cols / 2, frame.rows / 2));

    cv::cvtColor(output, gray, cv::COLOR_BGR2GRAY);

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

bool HumanDetector::operator==(const HumanDetector & other) const
{
    return this == &other;
}
