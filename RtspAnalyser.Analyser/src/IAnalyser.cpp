#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Streamer/Streamer.h"

using namespace Nico::RtspAnalyser::Analyser;

void IAnalyser::subscribe(const Streamer::Streamer & streamer) {
    streamer.subscribe(*this);
}
