#pragma once

#include <string>
#include <chrono>
#include "DelNico/RtspAnalyser/Libs/Codec.h"


namespace DelNico::RtspAnalyser::Libs {
    struct Stream {
        std::string url;
        Codec codec;
        std::chrono::microseconds frequency;
    };
}

