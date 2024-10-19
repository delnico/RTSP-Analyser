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
        this->streams.push_back(s);
    }
}

int Config::getHowManyStreams() const {
    return streams.size();
}

std::string Config::getStreamUrl(int index) const {
    return "rtsp://" + nvr_user + ":" + nvr_password + "@" + nvr_ip + ":" + std::to_string(nvr_port) + streams[index].url;
}

Codec Config::getStreamCodec(int index) const {
    return streams[index].codec;
}

Config::~Config(){}
