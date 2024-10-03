#include <iostream>
#include <memory>

#include "Nico/RtspAnalyser/RtspAnalyser.h"
#include "Nico/RtspAnalyser/Libs/Config.h"
#include "Nico/RtspAnalyser/Libs/Stream.h"
#include "Nico/RtspAnalyser/Libs/Codec.h"
#include "Nico/RtspAnalyser/Streamer/Streamer.h"
#include "Nico/RtspAnalyser/Analyser/Viewer.h"

using namespace Nico::RtspAnalyser;
using namespace Nico::RtspAnalyser::Libs;
using namespace Nico::RtspAnalyser::Streamers;
using namespace Nico::RtspAnalyser::Analyser;

int main(int argc, char* argv[])
{
    Config conf("/mnt/data/dev/perso/rstp_analyser/rtsp_config.json");

    Stream stream;
    stream.url = conf.getStreamUrl(0);
    stream.codec = conf.getStreamCodec(0);

    Streamer streamer(stream);

    Viewer viewer;

    streamer.subscribe(&viewer);

    viewer.start();
    streamer.start();

    std::cin.get();

    return EXIT_SUCCESS;
}
