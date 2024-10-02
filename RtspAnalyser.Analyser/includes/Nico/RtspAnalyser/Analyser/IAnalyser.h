
#pragma once

#include "Nico/RtspAnalyser/Streamer/Streamer.h"

namespace Nico {
    namespace RtspAnalyseer {
        namespace Analyser {
            // Analyser interface
            class IAnalyser {
                public:
                    virtual ~IAnalyser() = default;
                    void subscribe(const Streamer::Streamer & streamer);
            };
        }
    }
}
