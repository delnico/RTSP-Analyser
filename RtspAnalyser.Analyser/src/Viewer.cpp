#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>
#include <zmq.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/Viewer.h"

using namespace DelNico::RtspAnalyser::Analyser;

Viewer::Viewer(std::deque<cv::Mat> & frames, std::string socket_bind, zmq::context_t & zmqContext) :
    cond(),
    isEnabled(false),
    thread(),
    frames(frames),
    socket(zmqContext, zmq::socket_type::pull)
{
    socket.bind(socket_bind);
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
    cv::Mat frame;
    while (isEnabled)
    {
        cond.wait();
        if(frames.empty())
            continue;
        frame = frames.front();
        frames.pop_front();

        /*int metadata[3] = {frame.rows, frame.cols, frame.type()};
        zmq::message_t meta_msg(sizeof(metadata));
        memcpy(meta_msg.data(), metadata, sizeof(metadata));
        socket.send(meta_msg, zmq::send_flags::sndmore);*/

        try {
            zmq::message_t data_message(frame.total() * frame.elemSize());
            memcpy(data_message.data(), frame.datastart, data_message.size());
            socket.send(data_message, zmq::send_flags::none);
        }
        catch(const zmq::error_t& e) {
            std::cerr   << "errno=" << e.num()
                        << " what=" << e.what() << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        


        // TODO : send via ZMQ socket

        //imshow(socket_bind, frame);
        //cv::waitKey(1);                 // No pause, scheduled by Streamer thread
    }
    //cv::destroyWindow(socket_bind);
}

void Viewer::notify()
{
    cond.notify();
}


bool Viewer::operator==(const Viewer & other) const
{
    return &other == this;
}

