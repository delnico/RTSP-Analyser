#pragma once

#include <string>
#include "Nico/RtspAnalyser/Libs/Codec.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Libs {
            struct Stream {
                std::string url;
                Codec codec;
            };
        }
    }
}