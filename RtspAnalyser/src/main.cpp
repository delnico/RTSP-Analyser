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
        setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "rtsp_transport;tcp", 1);

        VideoCapture cap(uri, CAP_FFMPEG);
        HOGDescriptor hog;
        hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

        vector<Point> track;
        
        if (!cap.isOpened()) {           
            std::cerr << "Tentative de connection au flux RTSP." << std::endl;
            return EXIT_FAILURE;
        }

        cv::Mat frame;
        cv::Mat analyseFrame;

        while (true) {
            cap >> frame;

            if (frame.empty()) {
                std::cerr << "Frame vide." << std::endl;
            }
            else {

                std::vector<cv::Rect> found, found_filtered;
                analyseFrame = frame.clone();
                resize(analyseFrame, analyseFrame, Size(640, 480));
                hog.detectMultiScale(analyseFrame, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);

                size_t i, j;
                for (i = 0; i < found.size(); i++) {
                    cv::Rect r = found[i];
                    for (j = 0; j < found.size(); j++)
                        if (j != i && (r & found[j]) == r)
                            break;
                    if (j == found.size())
                        found_filtered.push_back(r);
                }

                for (i = 0; i < found_filtered.size(); i++) {
                    cv::Rect r = found_filtered[i];
                    r.x += cvRound(r.width * 0.1);
                    r.width = cvRound(r.width * 0.8);
                    r.y += cvRound(r.height * 0.07);
                    r.height = cvRound(r.height * 0.8);
                    cv::rectangle(frame, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
                }



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