#pragma once

#include <deque>

#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include <zmq.hpp>

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
    class StreamAnalyserHandler {
        public:
            StreamAnalyserHandler(
                boost::asio::io_context & io_service,
                zmq::context_t & zmqContext,
                const Stream & stream,
                Logger * logger,
                TriggerWorker * triggerWorker,
                Config & conf,
                int stream_id,
                std::string dbg_stream_ipc_main = "",
                std::string dbg_stream_ipc_fgmask = "",
                std::string dbg_stream_ipc_hdoutput = ""
            );
            ~StreamAnalyserHandler();

            void start(
                boost::asio::io_context & io_service,
                std::string nvr_ip,
                int nvr_port,
                std::string nvr_user,
                std::string nvr_password,
                std::string stream_path,
                std::string gstreamer_pipeline_params
            );
            void stop();
        
        private:
            std::deque<cv::Mat> stream_frames, viewer_frames, motio_detect_frames, fgMasks, human_detect_frames, human_detector_output;

            StreamReceiver streamReceiver;
            Multiplexer multiplexer;
            Streamer * streamerMain, * streamerFgMasks, * streamerHDOutput;
            MotionManager motionManager;
            MotionDetector motionDetector;
            HumanDetector humanDetector;
            OutputStream * os_viewer, os_motiondetector, os_human_detector;
            DelNico::RtspAnalyser::Watchdog::Watchdog watchdog;
    };
}
