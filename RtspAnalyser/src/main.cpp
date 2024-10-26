#include <iostream>
#include <memory>
#include <deque>
#include <thread>

#include <opencv2/opencv.hpp>

#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include "Nico/RtspAnalyser/RtspAnalyser.h"
#include "Nico/RtspAnalyser/Libs/Config.h"
#include "Nico/RtspAnalyser/Libs/Stream.h"
#include "Nico/RtspAnalyser/Libs/Codec.h"
#include "Nico/RtspAnalyser/Streamers/Streamer.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"
#include "Nico/RtspAnalyser/Motion/MotionDetector.h"

#include "Nico/RtspAnalyser/WatchdogLib/Watchdog.h"

using namespace Nico::RtspAnalyser;
using namespace Nico::RtspAnalyser::Libs;
using namespace Nico::RtspAnalyser::Streamers;
using namespace Nico::RtspAnalyser::Analyser;
using namespace Nico::RtspAnalyser::Motion;
using namespace Nico::RtspAnalyser::WatchdogLib;

int main(int argc, char* argv[])
{
    //cv::setNumThreads(0);

    std::string configFile = "config.json";
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("config,c", boost::program_options::value<std::string>(), "json configuration file");
    
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if(vm.count("help")) {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    if(vm.count("config")) {
        configFile = vm["config"].as<std::string>();
    }

    Config conf(configFile);

    boost::asio::io_service boost_io_service;

    Stream stream;
    stream.url = conf.getStreamUrl(0);
    stream.codec = conf.getStreamCodec(0);
    stream.frequency = std::chrono::microseconds(1000000LL / 30000 * 1000);

    std::deque<cv::Mat> frames, fgMasks;

    Streamer streamer(
        boost_io_service,
        stream,
        frames
    );

    //Viewer viewer(frames, "rtsp");
    Viewer viewerFgMasks(fgMasks, "fgMasks");

    MotionDetector motionDetector(
        frames,
        fgMasks,
        1000LL / 30
    );
    motionDetector.setViewer(&viewerFgMasks);

    Watchdog watchdog(&streamer, &motionDetector);
    watchdog.start();

    //streamer.subscribe(&viewer);

    streamer.subscribe(&motionDetector);

    viewerFgMasks.start();
    //viewer.start();

    motionDetector.start();
    streamer.start(boost_io_service);

    std::thread boost_io_thread(
        [](boost::asio::io_service & io) {
            io.run();
        },
        std::ref(boost_io_service)
    );

    std::cin.get();

    streamer.stop();
    motionDetector.stop();
    //viewer.stop();
    viewerFgMasks.stop();
    watchdog.stop();

    boost_io_service.stop();
    boost_io_thread.join();

    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}
