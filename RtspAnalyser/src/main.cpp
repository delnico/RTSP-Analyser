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
#include "Nico/RtspAnalyser/Analyser/Multiplexer.h"
#include "Nico/RtspAnalyser/Analyser/OutputStream.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"
#include "Nico/RtspAnalyser/Motion/MotionDetector.h"

#include "Nico/RtspAnalyser/WatchdogLib/Watchdog.h"
#include "Nico/RtspAnalyser/Libs/Logger.h"

using namespace Nico::RtspAnalyser;
using namespace Nico::RtspAnalyser::Libs;
using namespace Nico::RtspAnalyser::Streamers;
using namespace Nico::RtspAnalyser::Analyser;
using namespace Nico::RtspAnalyser::Motion;
using namespace Nico::RtspAnalyser::WatchdogLib;

int main(int argc, char* argv[])
{
    cv::setNumThreads(1);

    std::string configFile = "config.json";
    std::string logFile = "";
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("config,c", boost::program_options::value<std::string>(), "json configuration file")
        ("log,l", boost::program_options::value<std::string>(), "log file");
    
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

    if(vm.count("log")) {
        logFile = vm["log"].as<std::string>();
    }

    Config conf(configFile);

    if(logFile == "") {
        logFile = conf.getLogsFilePath();
    }

    boost::asio::io_service boost_io_service;

    Stream stream;
    stream.url = conf.getStreamUrl(0);
    stream.codec = conf.getStreamCodec(0);
    stream.frequency = std::chrono::microseconds(1000000LL / 30000 * 1000);

    std::deque<cv::Mat> stream_frames, viewer_frames, motio_detect_frames, fgMasks;

    Logger logger(logFile);
    logger.start();

    Streamer streamer(
        boost_io_service,
        stream,
        stream_frames
    );

    Multiplexer multiplexer(stream_frames);

    Viewer viewer(viewer_frames, "rtsp");

    Viewer viewerFgMasks(fgMasks, "fgMasks");

    MotionDetector motionDetector(
        conf,
        motio_detect_frames,
        fgMasks,
        30
    );
    motionDetector.setViewer(&viewerFgMasks);

    OutputStream os_viewer(&viewer, viewer_frames, 1);
    OutputStream os_motiondetector(&motionDetector, motio_detect_frames, 3);

    multiplexer.subscribe(&os_viewer);
    multiplexer.subscribe(&os_motiondetector);

    multiplexer.start();

    Watchdog watchdog(
        &streamer,
        &motionDetector,
        &logger
    );
    watchdog.start();

    streamer.subscribe(&multiplexer);

    viewer.start();
    viewerFgMasks.start();

    motionDetector.start();
    streamer.start(boost_io_service);

    std::thread boost_io_thread(
        [](boost::asio::io_service & io) {
            io.run();
        },
        std::ref(boost_io_service)
    );


    std::string input;
    bool enabled = true;
    while(enabled) {
        std::cin >> input;
        switch(input[0]) {
            case 'q':
                enabled = false;
                break;
            case 'r':
                motionDetector.reloadConfig(conf);
                break;
            default:
                break;
        }
    }

    streamer.stop();
    motionDetector.stop();
    viewer.stop();
    viewerFgMasks.stop();
    watchdog.stop();

    multiplexer.stop();

    boost_io_service.stop();
    boost_io_thread.join();

    cv::destroyAllWindows();

    logger.stop();

    return EXIT_SUCCESS;
}
