#pragma once

#include <string>
#include <chrono>


namespace DelNico::RtspAnalyser::Libs {
    struct Stream {
        std::string url;
        std::chrono::microseconds frequency;
    };
}

