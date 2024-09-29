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
#include <nlohmann/json.hpp>
#include <fstream>
#include <exception>

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

    // Define globally the number of threads to use for OpenCV, not thread safe !
    //cv::setNumThreads(1);

    VideoCapture cap;

    if(cap.open(uri, CAP_FFMPEG)) {
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

    cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::createBackgroundSubtractorMOG2();

    cv::Mat frame, fgMask, roiMask;
    cv::Rect zone(0, 0, 1280, 720);

    while (true) {

        try{
            cap.read(frame);
        }
        catch (cv::Exception& e) {
            std::cerr << "Erreur lors de la lecture du flux RTSP." << std::endl;
            std::cerr << e.what() << std::endl;
            break;
        }
        catch (std::exception& e) {
            std::cerr << "Erreur lors de la lecture du flux RTSP." << std::endl;
            std::cerr << e.what() << std::endl;
            break;
        }
        catch (...) {
            std::cerr << "Erreur lors de la lecture du flux RTSP." << std::endl;
            break;
        }

        if (frame.empty()) {
            std::cerr << "Frame vide." << std::endl;
            break;
        }

        bgSubtractor->apply(frame, fgMask);

        roiMask = cv::Mat::zeros(fgMask.size(), fgMask.type());
        roiMask(zone) = fgMask(zone);

        cv::rectangle(frame, zone, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Mouvement dans la zone", roiMask);
        
        cv::imshow("RTSP Stream", frame);

        if (cv::waitKey(33) == 27) {  // Waiting for 33 ms = ~ 30 frames per second (ideally 1000/30 = 33.33 ms)
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();    

    return EXIT_SUCCESS;
}
