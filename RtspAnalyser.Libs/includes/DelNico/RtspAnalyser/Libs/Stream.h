#pragma once

#include <string>
#include <chrono>

#include <opencv2/opencv.hpp>


namespace DelNico::RtspAnalyser::Libs {
    struct Stream {
        std::string url;
        std::chrono::microseconds frequency;
        std::vector<cv::Rect> zones;
    };
}

