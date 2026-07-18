#include <iostream>
#include <memory>
#include <thread>
#include <list>

#include <opencv2/opencv.hpp>

#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include <zmq.hpp>

#include "DelNico/RtspAnalyser/RtspAnalyser.h"
#include "DelNico/RtspAnalyser/StreamAnalyserHandler.h"
#include "DelNico/RtspAnalyser/Analyser/Streamer.h"
#include "DelNico/RtspAnalyser/Analyser/TriggerWorker.h"
#include "DelNico/RtspAnalyser/Libs/Config.h"
#include "DelNico/RtspAnalyser/Libs/Stream.h"
#include "DelNico/RtspAnalyser/Libs/Logger.h"

using namespace DelNico::RtspAnalyser;
using namespace DelNico::RtspAnalyser::Analyser;
using namespace DelNico::RtspAnalyser::Libs;
using namespace DelNico::RtspAnalyser::Motion;


int main(int argc, char* argv[])
{
    // cv::setNumThreads(1);

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

    boost::asio::io_context boost_io_service;

    zmq::context_t zmqContext = zmq::context_t();

    std::list<Stream> streams = conf.getStreams();

    Logger logger(logFile);
    logger.start();

    TriggerWorker triggerWorker(
        conf.get<std::string>("smtp_server"),
        conf.get<int>("smtp_port"),
        conf.get<std::string>("smtp_username")
    );

    std::list<std::unique_ptr<StreamAnalyserHandler>> sahs;

    bool debug_enabled = conf.get<bool>("debug_enabled");

    int stream_count = 1;   // start at 1 : more user friendly, of course for debug start at 1 in json config
    if(debug_enabled) {
        int dbg_stream_id = conf.get<int>("dbg_stream_id");
        for (auto & stream : streams) {
            if(stream_count == dbg_stream_id) {
                auto sah = std::make_unique<StreamAnalyserHandler>(
                    boost_io_service,
                    zmqContext,
                    stream,
                    &logger,
                    &triggerWorker,
                    conf,
                    stream_count,
                    conf.get<std::string>("dbg_stream_main"),
                    conf.get<std::string>("dbg_stream_fgmask"),
                    conf.get<std::string>("dbg_stream_hd")
                );
                sah->start(
                    boost_io_service,
                    conf.get<std::string>("nvr_ip"),
                    conf.get<int>("nvr_port"),
                    conf.get<std::string>("nvr_user"),
                    conf.get<std::string>("nvr_password"),
                    stream.url,
                    conf.get<std::string>("nvr_gstreamer_pipeline_params")
                );
                sahs.push_back(
                    std::move(sah)
                );
            }
            else {
                auto sah = std::make_unique<StreamAnalyserHandler>(
                    boost_io_service,
                    zmqContext,
                    stream,
                    &logger,
                    &triggerWorker,
                    conf,
                    stream_count
                );
                sah->start(
                    boost_io_service,
                    conf.get<std::string>("nvr_ip"),
                    conf.get<int>("nvr_port"),
                    conf.get<std::string>("nvr_user"),
                    conf.get<std::string>("nvr_password"),
                    stream.url,
                    conf.get<std::string>("nvr_gstreamer_pipeline_params")
                );
                sahs.push_back(
                    std::move(sah)
                );
            }
            stream_count++;
        }
    }
    else {
        for (auto & stream : streams) {
            auto sah = std::make_unique<StreamAnalyserHandler>(
                boost_io_service,
                zmqContext,
                stream,
                &logger,
                &triggerWorker,
                conf,
                stream_count
            );
            sah->start(
                boost_io_service,
                conf.get<std::string>("nvr_ip"),
                conf.get<int>("nvr_port"),
                conf.get<std::string>("nvr_user"),
                conf.get<std::string>("nvr_password"),
                stream.url,
                conf.get<std::string>("nvr_gstreamer_pipeline_params")
            );
            sahs.push_back(
                std::move(sah)
            );
            stream_count++;
        }
    }

    triggerWorker.start();
    

    std::thread boost_io_thread(
        [](boost::asio::io_context & io) {
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
            default:
                break;
        }
    }

    for (auto & sah : sahs) {
        sah->stop();
    }

    triggerWorker.stop();

    boost_io_service.stop();
    boost_io_thread.join();

    zmqContext.close();

    logger.stop();

    return EXIT_SUCCESS;
}
