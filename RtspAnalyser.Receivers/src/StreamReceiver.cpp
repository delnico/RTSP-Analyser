#include <atomic>
#include <string>
#include <deque>
#include <thread>
#include <vector>
#include <memory>
#include <cstdint>

#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Libs/Stream.h"
#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Receivers/StreamReceiver.h"


namespace DelNico::RtspAnalyser::Receivers {
    
    StreamReceiver::StreamReceiver(
        boost::asio::io_service & io_service,
        const Libs::Stream & stream,
        std::deque<cv::Mat> & frames
    ) :
        isEnabled(false),
        timer(boost::asio::deadline_timer(io_service, boost::posix_time::microsec(stream.frequency.count()))),
        stream(stream),
        cap(),
        frames(frames),
        gstreamer_pipeline("")
    {
    }

    StreamReceiver::~StreamReceiver()
    {
        stop();
    }

    void StreamReceiver::start(
    boost::asio::io_service & io_service,
    std::string nvr_ip,
    int nvr_port,
    std::string nvr_user,
    std::string nvr_password,
    std::string stream_path,
    std::string gstreamer_pipeline_params
)
{
    std::string base_url = "rtsp://" + nvr_ip + ":" + std::to_string(nvr_port) + stream_path;

    gstreamer_pipeline = 
        "rtspsrc location=\"" + base_url + "\" "
        "user-id=\"" + nvr_user + "\" "
        "user-pw=\"" + nvr_password + "\" "
        + gstreamer_pipeline_params;

    cap.open(gstreamer_pipeline, cv::CAP_GSTREAMER);

    if(!cap.isOpened())
        throw std::runtime_error("Failed to open stream with GStreamer");
    
    timer = boost::asio::deadline_timer(io_service, boost::posix_time::microsec(stream.frequency.count()));
    timer.async_wait(boost::bind(&StreamReceiver::run, this));
}

    void StreamReceiver::stop()
    {
        if(isEnabled.load())
        {
            isEnabled.store(false);
            timer.cancel();
        }
    }

    void StreamReceiver::subscribe(Analyser::IAnalyser * analyser)
    {
        listener = analyser;
    }

    void StreamReceiver::unsubscribe(Analyser::IAnalyser * analyser)
    {
        listener = nullptr;
    }

    void StreamReceiver::watchdog()
    {
        if(queueSize() > 3) {
            goToLive();
        }
    }

    int64_t StreamReceiver::queueSize() const
    {
        return frames.size();
    }

    void StreamReceiver::goToLive()
    {
        // let clear frames queue but keep the last frame (and one before, for secure queue)
        while(frames.size() > 2)
        {
            frames.pop_front();
        }
    }

    void StreamReceiver::run()
    {
        cv::Mat frame;
        std::chrono::duration<double> elapsed(0);
        
        auto start = std::chrono::steady_clock::now();
        try
        {
            cap >> frame;
            if(! frame.empty() && frame.size().width > 0 && frame.size().height > 0)
            {
                if(listener != nullptr)
                {
                    frames.push_back(frame);
                    listener->notify();
                }
            }
        }
        catch(const std::exception & e)
        {
            Libs::Logger::log_main(std::format("StreamReceiver : {} : have crash, try restart stream", stream.url));
            autoReloadAfterCrash();
        }
        auto end = std::chrono::steady_clock::now();
        elapsed = end - start;
        auto sleep_duration = std::chrono::microseconds((int64_t) (stream.frequency.count() - (elapsed.count() * 1000)));
        timer.expires_at(timer.expires_at() + boost::posix_time::microsec(sleep_duration.count()));
        timer.async_wait(boost::bind(&StreamReceiver::run, this));
    }

    void StreamReceiver::autoReloadAfterCrash()
    {
        try
        {
            if(isEnabled.load())
            {
                isEnabled.store(false);
                timer.cancel();
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            cap.open(gstreamer_pipeline, cv::CAP_GSTREAMER);
            if(!cap.isOpened())
                throw std::runtime_error("Failed to open stream with GStreamer");
        }
        catch(const std::exception & e)
        {
            Libs::Logger::log_main(std::format("StreamReceiver : {} : crash during restart, try again in 5sec", stream.url));
            timer.expires_at(timer.expires_at() + boost::posix_time::microsec(std::chrono::seconds(5).count()));
            timer.async_wait(boost::bind(&StreamReceiver::autoReloadAfterCrash, this));
        }
        auto sleep_duration = std::chrono::microseconds((int64_t) (stream.frequency.count()));
        timer.expires_at(timer.expires_at() + boost::posix_time::microsec(sleep_duration.count()));
        timer.async_wait(boost::bind(&StreamReceiver::run, this));
    }
}
