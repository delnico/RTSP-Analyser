#include <iostream>
#include <memory>
#include <deque>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/RtspAnalyser.h"
#include "Nico/RtspAnalyser/Libs/Config.h"
#include "Nico/RtspAnalyser/Libs/Stream.h"
#include "Nico/RtspAnalyser/Libs/Codec.h"
#include "Nico/RtspAnalyser/Streamer/Streamer.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"
#include "Nico/RtspAnalyser/Motion/MotionDetector.h"

using namespace Nico::RtspAnalyser;
using namespace Nico::RtspAnalyser::Libs;
using namespace Nico::RtspAnalyser::Streamers;
using namespace Nico::RtspAnalyser::Analyser;
using namespace Nico::RtspAnalyser::Motion;

int main(int argc, char* argv[])
{
    //cv::setNumThreads(0);


    Config conf("config.json");

    Stream stream;
    stream.url = conf.getStreamUrl(0);
    stream.codec = conf.getStreamCodec(0);

    std::deque<cv::Mat> frames, fgMasks;

    Streamer streamer(stream, frames);

    //Viewer viewer(frames, "rtsp");
    Viewer viewerFgMasks(fgMasks, "fgMasks");

    MotionDetector motionDetector(frames, fgMasks);
    motionDetector.setViewer(&viewerFgMasks);

    //streamer.subscribe(&viewer);

    streamer.subscribe(&motionDetector);

    viewerFgMasks.start();
    //viewer.start();

    motionDetector.start();
    streamer.start();

    std::cin.get();

    streamer.stop();
    motionDetector.stop();
    //viewer.stop();
    viewerFgMasks.stop();

    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}
