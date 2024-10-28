#pragma once

#include <deque>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Analyser {
            struct OutputStream {
                Nico::RtspAnalyser::Analyser::IAnalyser * output;
                std::deque<cv::Mat> frames;
                int64_t frame_skipping;
            };
        }
    }
}