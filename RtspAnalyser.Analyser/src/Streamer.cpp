#include <thread>
#include <atomic>
#include <deque>

#include <opencv2/opencv.hpp>
#include <zmq.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/Streamer.h"

using namespace DelNico::RtspAnalyser::Analyser;

Streamer::Streamer(std::deque<cv::Mat> & frames, std::string socket_bind, zmq::context_t & zmqContext) :
    cond(),
    isEnabled(false),
    thread(),
    frames(frames),
    socket(zmqContext, zmq::socket_type::push)
{
    socket.connect(socket_bind);
}

Streamer::~Streamer()
{
    stop();
}

void Streamer::start()
{
    isEnabled.store(true);
    thread = std::thread(&Streamer::run, this);
}

void Streamer::stop()
{
    if(thread.joinable())
    {
        isEnabled.store(false);
        notify();
        thread.join();
        socket.close();
    }
}

void Streamer::run()
{
    cv::Mat frame;
    while (isEnabled)
    {
        cond.wait();
        if(frames.empty())
            continue;
        frame = frames.front();
        frames.pop_front();

        try {
            int metadata[4] = {frame.rows, frame.cols, frame.channels(), frame.type()};
            zmq::message_t meta_msg(sizeof(metadata));
            memcpy(meta_msg.data(), metadata, sizeof(metadata));
            socket.send(meta_msg, zmq::send_flags::sndmore);

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
    }
}

void Streamer::notify()
{
    cond.notify();
}


bool Streamer::operator==(const Streamer & other) const
{
    return &other == this;
}

