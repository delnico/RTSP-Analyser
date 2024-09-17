#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <gst/gst.h>
#include <nlohmann/json.hpp>
#include <fstream>

#include "Nico/RtspAnalyser/RtspAnalyser.h"
#include "Nico/RtspAnalyser/Libs/Config.h"
#include "Nico/RtspAnalyser/Libs/Codec.h"

using namespace std;
using namespace Nico::RtspAnalyser;
using namespace Nico::RtspAnalyser::Libs;
using namespace cv;
using json = nlohmann::json;

int main(int argc, char* argv[])
{
    const string rtsp_config_file = "/mnt/data/dev/perso/rstp_analyser/rtsp_config.json";
    // read json file
    Config conf(rtsp_config_file);

    std::cout << "Streams: " << std::endl;
    for(int i = 0; i < conf.getHowManyStreams(); i++) {
        std::cout << "\t" << i << " : " << conf.getStreamUrl(i) << "\t" << conf.getStreamCodec(i) << std::endl;
    }

    const string uri = conf.getStreamUrl(0);

    // GStreamer pipeline for read RTSP stream encoded with H265, output with BGR, and display it, with TCP protocol
    std::string pipeline = "rtspsrc location=" + uri + " protocols=\"tcp\" latency=2000 ! application/x-rtp, media=video ! rtph265depay ! h265parse ! avdec_h265 ! video/x-raw ! autovideosink ! appsink";

    std::cout << "Pipeline: " << std::endl << pipeline << std::endl;

    VideoCapture cap;

    if(cap.open(pipeline, CAP_GSTREAMER)) {
        std::cout << "Connection au flux RTSP établie." << std::endl;
    }
    else {
        std::cerr << "Tentative de connection au flux RTSP." << std::endl;
        return EXIT_FAILURE;
    }
    
    if (!cap.isOpened()) {           
        std::cerr << "Tentative de connection au flux RTSP." << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat frame;

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "Frame vide." << std::endl;
        }
        else {
            cv::imshow("RTSP Stream", frame);
        }

        if (cv::waitKey(30) == 27) {  // Attendre 30 ms ou jusqu'à ce que la touche 'Esc' soit enfoncée
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();    

    return EXIT_SUCCESS;
}
