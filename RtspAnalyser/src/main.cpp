#include <iostream>
#include <memory>
#include <deque>
#include <thread>

#include <opencv2/opencv.hpp>

#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include <zmq.hpp>

#include "DelNico/RtspAnalyser/RtspAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/HumanDetector.h"
#include "DelNico/RtspAnalyser/Analyser/Multiplexer.h"
#include "DelNico/RtspAnalyser/Analyser/OutputStream.h"
#include "DelNico/RtspAnalyser/Analyser/Viewer.h"
#include "DelNico/RtspAnalyser/Libs/Config.h"
#include "DelNico/RtspAnalyser/Libs/Stream.h"
#include "DelNico/RtspAnalyser/Libs/Codec.h"
#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Motion/MotionDetector.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"
#include "DelNico/RtspAnalyser/Streamers/Streamer.h"
#include "DelNico/RtspAnalyser/WatchdogLib/Watchdog.h"

using namespace DelNico::RtspAnalyser;
using namespace DelNico::RtspAnalyser::Analyser;
using namespace DelNico::RtspAnalyser::Libs;
using namespace DelNico::RtspAnalyser::Motion;
using namespace DelNico::RtspAnalyser::Streamers;
using namespace DelNico::RtspAnalyser::WatchdogLib;


int main(int argc, char* argv[])
{
    cv::setNumThreads(1);

    std::string configFile = "config.json";
    std::string logFile;
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("config,c", boost::program_options::value<std::string>(), "json configuration file");
    
    boost::program_options::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if(vm.count("help")) {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    if(vm.count("config")) {
        configFile = vm["config"].as<std::string>();
    }

    Config conf(configFile);

    if(logFile.empty()) {
        logFile = conf.get<std::string>("log_path");
    }

    boost::asio::io_service boost_io_service;

    zmq::context_t zmqContext = zmq::context_t();

    Stream stream;
    stream.url = conf.getStreamUrl(0);
    stream.codec = conf.getStreamCodec(0);
    stream.frequency = std::chrono::microseconds(1000000LL / 30000 * 1000);

    std::deque<cv::Mat> stream_frames, viewer_frames, motio_detect_frames, fgMasks, human_detect_frames, human_detector_output;

    Logger logger(logFile);
    logger.start();

    Streamer streamer(
        boost_io_service,
        stream,
        stream_frames
    );

    Multiplexer multiplexer(stream_frames);

    Viewer viewer(viewer_frames, "ipc:///tmp/rtsp_main.zmq", zmqContext);

    Viewer viewerFgMasks(fgMasks, "ipc:///tmp/rtsp_fgmask.zmq", zmqContext);

    Viewer viewerHDOutput(human_detector_output, "ipc:///tmp/video_hdoutput.zmq", zmqContext);

    MotionManager motionManager(
        boost_io_service,
        std::chrono::seconds(150),
        &multiplexer
    );

    MotionDetector motionDetector(
        conf,
        motio_detect_frames,
        fgMasks,
        30
    );
    motionDetector.setViewer(&viewerFgMasks);
    motionDetector.setMotionManager(&motionManager);

    HumanDetector humanDetector(human_detect_frames, &motionManager);
    humanDetector.setViewer(&viewerHDOutput, &human_detector_output);

    OutputStream os_viewer(&viewer, viewer_frames, 1);
    OutputStream os_motiondetector(&motionDetector, motio_detect_frames, 3);
    OutputStream os_human_detector(&humanDetector, human_detect_frames, 3);

    multiplexer.subscribe(&os_viewer);
    multiplexer.subscribe(&os_motiondetector);
    multiplexer.set_stream_redirect_client(&os_human_detector);

    motionManager.start();

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
    viewerHDOutput.start();

    motionDetector.start();
    humanDetector.start();
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
    humanDetector.stop();
    viewer.stop();
    viewerFgMasks.stop();
    viewerHDOutput.stop();

    watchdog.stop();

    multiplexer.stop();
    motionManager.stop();

    boost_io_service.stop();
    boost_io_thread.join();

    zmqContext.close();

    cv::destroyAllWindows();

    logger.stop();

    return EXIT_SUCCESS;
}
