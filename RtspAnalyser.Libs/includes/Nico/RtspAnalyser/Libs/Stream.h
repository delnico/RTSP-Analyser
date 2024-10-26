#pragma once

#include <string>
#include <chrono>
#include "Nico/RtspAnalyser/Libs/Codec.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Libs {
            struct Stream {
                std::string url;
                Codec codec;
                std::chrono::microseconds frequency;
            };
        }
    }
}