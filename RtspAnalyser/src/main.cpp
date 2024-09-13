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
#include "Nico/RtspAnalyser/RtspAnalyser.h"

using namespace std;
using namespace Nico::RtspAnalyser;
using namespace cv;

int main(int argc, char* argv[])
{
    const string uri = "rtsp://analyser:jkTRf8g6a7m6q_jkicxi@192.168.2.252:29172/unicast/c1/s1/live";

    try
    {
        //setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "rtsp_transport;tcp", 1);

        // setenv("GST_PLUGIN_PATH", "/mnt/data/dev/perso/rstp_analyser/build/vcpkg_installed/x64-linux/lib/gstreamer-1.0", 1);
        // setenv("GST_DEBUG", "3", 1);

        std::string pipeline = "rtspsrc location=" + uri + " ! h265parse ! avdec_h265 ! videoconvert ! appsink";

        VideoCapture cap(pipeline, CAP_GSTREAMER);
        
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
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return EXIT_SUCCESS;
}