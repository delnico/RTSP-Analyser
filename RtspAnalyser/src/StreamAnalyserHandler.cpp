#include <deque>

#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <zmq.hpp>

#include "DelNico/RtspAnalyser/StreamAnalyserHandler.h"
#include "DelNico/RtspAnalyser/Analyser/HumanDetector.h"
#include "DelNico/RtspAnalyser/Analyser/TriggerWorker.h"
#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Libs/Stream.h"
#include "DelNico/RtspAnalyser/Motion/MotionDetector.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"
#include "DelNico/RtspAnalyser/Receivers/StreamReceiver.h"
#include "DelNico/RtspAnalyser/Watchdog/Watchdog.h"

using namespace DelNico::RtspAnalyser::Analyser;
using namespace DelNico::RtspAnalyser::Libs;
using namespace DelNico::RtspAnalyser::Motion;
using namespace DelNico::RtspAnalyser::Receivers;
using namespace DelNico::RtspAnalyser::Watchdog;

namespace DelNico::RtspAnalyser {
    StreamAnalyserHandler::StreamAnalyserHandler(
        boost::asio::io_context & io_service,
        zmq::context_t & zmqContext,
        const Stream & stream,
        Logger * logger,
        TriggerWorker * triggerWorker,
        Config & conf,
        int stream_id,
        std::string dbg_stream_ipc_main,
        std::string dbg_stream_ipc_fgmask,
        std::string dbg_stream_ipc_hdoutput
    ) :
        streamReceiver(io_service, stream, stream_frames),
        multiplexer(stream_frames),
        streamerMain(nullptr),
        streamerFgMasks(nullptr),
        streamerHDOutput(nullptr),
        motionManager(io_service, std::chrono::seconds(150), &multiplexer, triggerWorker, stream_id),
        motionDetector(conf, motio_detect_frames, fgMasks, 30, stream.zones),
        humanDetector(human_detect_frames, &motionManager, stream.zones, conf.get<float>("human_detection_min_score")),
        os_viewer(nullptr),
        os_motiondetector(&motionDetector, motio_detect_frames, 5),
        os_human_detector(&humanDetector, human_detect_frames, 5),
        watchdog(
            logger
        )
    {
        if(dbg_stream_ipc_main.length() > 0) {
            streamerMain = new Streamer(viewer_frames, dbg_stream_ipc_main, zmqContext);
        }
        if(dbg_stream_ipc_fgmask.length() > 0) {
            streamerFgMasks = new Streamer(fgMasks, dbg_stream_ipc_fgmask, zmqContext);
            motionDetector.setStreamer(streamerFgMasks);
        }
        if(dbg_stream_ipc_hdoutput.length() > 0) {
            streamerHDOutput = new Streamer(human_detector_output, dbg_stream_ipc_hdoutput, zmqContext);
            humanDetector.setStreamer(streamerHDOutput, &human_detector_output);
        }

        motionDetector.setMotionManager(&motionManager);

        if(streamerMain) {
            os_viewer = new OutputStream(streamerMain, viewer_frames, 1);
            multiplexer.subscribe(os_viewer);
        }
        multiplexer.subscribe(&os_motiondetector);
        multiplexer.set_stream_redirect_client(&os_human_detector);
    }

    StreamAnalyserHandler::~StreamAnalyserHandler() {
        if(os_viewer) {
            delete os_viewer;
        }
        if(streamerMain) {
            delete streamerMain;
        }
        if(streamerFgMasks) {
            delete streamerFgMasks;
        }
        if(streamerHDOutput) {
            delete streamerHDOutput;
        }
    }

    void StreamAnalyserHandler::start(
        boost::asio::io_context & io_service,
        std::string nvr_ip,
        int nvr_port,
        std::string nvr_user,
        std::string nvr_password,
        std::string stream_path,
        std::string gstreamer_pipeline_params
    ) {
        motionManager.start();
        multiplexer.start();
        watchdog.subscribe(std::bind(&MotionDetector::watchdog, &motionDetector));
        watchdog.subscribe(std::bind(&StreamReceiver::watchdog, &streamReceiver));
        watchdog.start();
        streamReceiver.subscribe(&multiplexer);
        if(streamerMain) {
            streamerMain->start();
        }
        if(streamerFgMasks) {
            streamerFgMasks->start();
        }
        if(streamerHDOutput) {
            streamerHDOutput->start();
        }
        motionDetector.start();
        humanDetector.start();
        streamReceiver.start(
            io_service,
            nvr_ip,
            nvr_port,
            nvr_user,
            nvr_password,
            stream_path,
            gstreamer_pipeline_params
        );
    }

    void StreamAnalyserHandler::stop() {
        streamReceiver.stop();
        motionDetector.stop();
        humanDetector.stop();
        if(streamerMain) {
            streamerMain->stop();
        }
        if(streamerFgMasks) {
            streamerFgMasks->stop();
        }
        if(streamerHDOutput) {
            streamerHDOutput->stop();
        }
        watchdog.stop();
        multiplexer.stop();
        motionManager.stop();
    }
}
