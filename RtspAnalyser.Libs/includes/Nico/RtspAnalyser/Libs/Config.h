#pragma once

#include <string>
#include <vector>
#include "Nico/RtspAnalyser/Libs/Codec.h"
#include "Nico/RtspAnalyser/Libs/Stream.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Libs {
            class Config {
                public:
                    Config();
                    Config(const std::string& file);
                    Config(const Config& orig) = delete;
                    Config& operator=(const Config& orig) = delete;
                    ~Config();
                    
                    int getHowManyStreams() const;
                    std::string getStreamUrl(int index) const;
                    Codec getStreamCodec(int index) const;

                    std::string getLogsFilePath() const;

                    int getOpenCvModelHistory() const;
                    int getOpenCvModelVarThreshold() const;
                    bool getOpenCvModelDetectShadows() const;
                    int getOpenCvModelErode() const;
                    int getOpenCvModelDilate() const;
                    int getOpenCvModelGaussianSize() const;
                    int getOpenCvModelGaussianSigma() const;
                    int getOpenCvModelMinArea() const;

                private:
                    std::string nvr_ip;
                    int nvr_port;
                    std::string nvr_user;
                    std::string nvr_password;
                    std::string nvr_protocol;
                    std::vector<struct Stream> streams;
                    std::string log_file_path;

                    int opencv_model_history;
                    int opencv_model_var_threshold;
                    bool opencv_model_detect_shadows;
                    int opencv_model_erode;
                    int opencv_model_dilate;
                    int opencv_model_gaussian_size;
                    int opencv_model_gaussian_sigma;
                    int opencv_model_min_area;
            };
        }
    }
}
