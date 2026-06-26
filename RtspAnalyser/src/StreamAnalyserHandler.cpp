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
        boost::asio::io_service & io_service,
        zmq::context_t & zmqContext,
        const Stream & stream,
        Logger * logger,
        TriggerWorker * triggerWorker,
        Config & conf
    ) :
        streamReceiver(io_service, stream, stream_frames),
        multiplexer(stream_frames),
        streamerMain(viewer_frames, "ipc:///tmp/rtsp_main.zmq", zmqContext),
        streamerFgMasks(fgMasks, "ipc:///tmp/rtsp_fgmask.zmq", zmqContext),
        streamerHDOutput(human_detector_output, "ipc:///tmp/video_hdoutput.zmq", zmqContext),
        motionManager(io_service, std::chrono::seconds(150), &multiplexer, triggerWorker, 1),
        motionDetector(conf, motio_detect_frames, fgMasks, 30),
        humanDetector(human_detect_frames, &motionManager),
        os_viewer(&streamerMain, viewer_frames, 1),
        os_motiondetector(&motionDetector, motio_detect_frames, 5),
        os_human_detector(&humanDetector, human_detect_frames, 5),
        watchdog(
            logger
        )
    {
        motionDetector.setStreamer(&streamerFgMasks);
        motionDetector.setMotionManager(&motionManager);

        humanDetector.setStreamer(&streamerHDOutput, &human_detector_output);

        multiplexer.subscribe(&os_viewer);
        multiplexer.subscribe(&os_motiondetector);
        multiplexer.set_stream_redirect_client(&os_human_detector);
    }

    void StreamAnalyserHandler::start(
        boost::asio::io_service & io_service,
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
        streamerMain.start();
        streamerFgMasks.start();
        streamerHDOutput.start();
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
        streamerMain.stop();
        streamerFgMasks.stop();
        streamerHDOutput.stop();
        watchdog.stop();
        multiplexer.stop();
        motionManager.stop();
    }
}