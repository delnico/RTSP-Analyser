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

    int history = 500;              // Nombre d'images pour le modèle de fond
    double varThreshold = 60;       // Seuil pour la sensibilité
    bool detectShadows = false;     // Désactiver la détection des ombres

    cv::Ptr<cv::BackgroundSubtractor> bgSubtractor = cv::createBackgroundSubtractorMOG2(history, varThreshold, detectShadows);

    cv::Mat frame, fgMask, roiMask;

    std::vector<cv::Rect> zones;
    zones.push_back(cv::Rect(0, 300, 1100, 420));
    zones.push_back(cv::Rect(500, 100, 650, 200));

    while (true) {
        try{
            cap.read(frame);

            cap >> frame;
            if (frame.empty()) break;

            bgSubtractor->apply(frame, fgMask);                         // Appliquer la soustraction d'arrière-plan pour obtenir le masque du premier plan

            roiMask = cv::Mat::zeros(fgMask.size(), fgMask.type());     // Initialiser un masque de la même taille que l'image originale

            for (const auto& zone : zones) {
                fgMask(zone).copyTo(roiMask(zone));                         // Copier uniquement la zone d'intérêt dans un nouveau masque

                // Appliquer un filtre morphologique pour éliminer le bruit (petites zones)
                cv::erode(roiMask, roiMask, cv::Mat(), cv::Point(-1, -1), 2);  // Réduire les petites zones
                cv::dilate(roiMask, roiMask, cv::Mat(), cv::Point(-1, -1), 2); // Agrandir les zones significatives

                cv::GaussianBlur(roiMask, roiMask, cv::Size(15, 15), 0); // Appliquer un flou pour lisser les petites variations

                // Trouver les contours dans le masque pour filtrer les petits mouvements
                std::vector<std::vector<cv::Point>> contours;
                cv::findContours(roiMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                
                // Filtrer les petits contours
                for (size_t i = 0; i < contours.size(); i++) {
                    if (cv::contourArea(contours[i]) > 500) { // Garder seulement les grands mouvements
                        cv::drawContours(frame, contours, (int)i, cv::Scalar(0, 0, 255), 2); // Dessiner les contours des grands objets en mouvement
                    }
                }
                    
                cv::rectangle(frame, zone, cv::Scalar(0, 255, 0), 2);       // Dessiner le rectangle de la zone sur l'image originale
            }

            //cv::imshow("Mouvement dans la zone", roiMask);              // Seulement afficher le mouvement détecté dans la zone d'intérêt
            cv::imshow("RTSP", frame);

            if (cv::waitKey(33) == 27) break;  // Waiting for 33 ms = ~ 30 frames per second (ideally 1000/30 = 33.33 ms)
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
    }

    cap.release();
    cv::destroyAllWindows();    

    return EXIT_SUCCESS;
}
