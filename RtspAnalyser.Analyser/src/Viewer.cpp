#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>
#include <zmq.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/Viewer.h"

using namespace DelNico::RtspAnalyser::Analyser;

Viewer::Viewer(std::deque<cv::Mat> & frames, std::string socket_bind) :
    cond(),
    isEnabled(false),
    thread(),
    socket_bind(socket_bind),
    frames(frames),
    zmqContext(1),
    zmqSocket(zmqContext, zmq::socket_type::pull)
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
    zmqSocket.bind(socket_bind);
    cv::Mat frame;
    while (isEnabled)
    {
        cond.wait();
        if(frames.empty())
            continue;
        frame = frames.front();
        frames.pop_front();
        imshow(socket_bind, frame);
        cv::waitKey(1);                 // No pause, scheduled by Streamer thread
    }
    cv::destroyWindow(socket_bind);
}

void Viewer::notify()
{
    cond.notify();
}


bool Viewer::operator==(const Viewer & other) const
{
    return &other == this;
}

