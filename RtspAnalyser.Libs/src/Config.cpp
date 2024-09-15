#include "Nico/RtspAnalyser/Libs/Config.h"
#include <fstream>
#include <nlohmann/json.hpp>

using namespace Nico::RtspAnalyser::Libs;
using json = nlohmann::json;
using namespace std;

Config::Config() : Config("./rtsp_config.json"){}

Config::Config(const std::string& file)
{
    std::ifstream ifs(file);
    json j = json::parse(ifs);

    if (j.find("nvr") != j.end()) {
        auto nvr = j["nvr"];
        nvr_ip = nvr["ip"];
        nvr_port = nvr["port"];
        nvr_user = nvr["user"];
        nvr_password = nvr["password"];
    }
}

int Config::getHowManyStreams() const
{
    return streams.size();
}

std::string Config::getStreamUrl(int index) const
{
    return "";
}

Config::~Config(){}
