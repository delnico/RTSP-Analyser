#include <fstream>
#include <nlohmann/json.hpp>

#include "Nico/RtspAnalyser/Libs/Config.h"
#include "Nico/RtspAnalyser/Libs/Codec.h"
#include "Nico/RtspAnalyser/Libs/Stream.h"

using namespace Nico::RtspAnalyser::Libs;
using json = nlohmann::json;
using namespace std;

Config::Config() : Config("config.json"){}

Config::Config(const std::string& file) {
    std::ifstream ifs(file);
    if(!ifs.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier de configuration.");
    }
    json j;
    ifs >> j;
    ifs.close();

    auto nvr = j["nvr"];
    nvr_ip = nvr["ip"];
    nvr_port = nvr["port"];
    nvr_user = nvr["username"];
    nvr_password = nvr["password"];

    auto streams = j["streams"];
    for(auto& stream : streams) {
        struct Stream s;
        s.url = stream["url"];
        std::string codec = stream["codec"];
        std::transform(codec.begin(), codec.end(), codec.begin(), ::tolower);
        if(codec == "h264") {
            s.codec = Codec::H264;
        } else if(codec == "h265") {
            s.codec = Codec::H265;
        } else {
            throw std::runtime_error("Codec inconnu.");
        }

        auto zones = stream["zones"];

        // for(auto& zone : zones)

        this->streams.push_back(s);
    }

    log_file_path = j["logs_file_path"];

    auto opencv_model_set = j["opencv_model_set"];
    opencv_model_history = opencv_model_set["history"];
    opencv_model_var_threshold = opencv_model_set["var_threshold"];
    opencv_model_detect_shadows = opencv_model_set["detect_shadows"];
    opencv_model_erode = opencv_model_set["erode"];
    opencv_model_dilate = opencv_model_set["dilate"];
    opencv_model_gaussian_size = opencv_model_set["gaussian_size"];
    opencv_model_gaussian_sigma = opencv_model_set["gaussian_sigma"];
    opencv_model_min_area = opencv_model_set["min_area"];
}

Config::~Config(){}

int Config::getHowManyStreams() const {
    return streams.size();
}

std::string Config::getStreamUrl(int index) const {
    return "rtsp://" + nvr_user + ":" + nvr_password + "@" + nvr_ip + ":" + std::to_string(nvr_port) + streams[index].url;
}

Codec Config::getStreamCodec(int index) const {
    return streams[index].codec;
}

std::string Config::getLogsFilePath() const {
    return log_file_path;
}


int Config::getOpenCvModelHistory() const {
    return opencv_model_history;
}

int Config::getOpenCvModelVarThreshold() const {
    return opencv_model_var_threshold;
}

bool Config::getOpenCvModelDetectShadows() const {
    return opencv_model_detect_shadows;
}

int Config::getOpenCvModelErode() const {
    return opencv_model_erode;
}

int Config::getOpenCvModelDilate() const {
    return opencv_model_dilate;
}

int Config::getOpenCvModelGaussianSize() const {
    return opencv_model_gaussian_size;
}

int Config::getOpenCvModelGaussianSigma() const {
    return opencv_model_gaussian_sigma;
}

int Config::getOpenCvModelMinArea() const {
    return opencv_model_min_area;
}
