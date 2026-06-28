#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "DelNico/RtspAnalyser/Libs/Config.h"
#include "DelNico/RtspAnalyser/Libs/Stream.h"

using namespace nlohmann;
using namespace std;

namespace DelNico::RtspAnalyser::Libs {

    Config::Config(const std::string& file) {
        std::ifstream ifs(file);
        if(!ifs.is_open()) {
            throw std::runtime_error("Impossible d'ouvrir le fichier de configuration.");
        }
        nlohmann::json j;
        ifs >> j;
        ifs.close();

        auto nvr = j["nvr"];
        nvr_ip = nvr["ip"];
        nvr_port = nvr["port"];
        nvr_user = nvr["username"];
        nvr_password = nvr["password"];
        nvr_gstreamer_pipeline_params = nvr["gstreamer_pipeline_params"];

        auto streams = j["streams"];
        for(auto& stream : streams) {
            struct Stream s;
            s.url = stream["url"];

            auto zones = stream["zones"];

            // for(auto& zone : zones)

            this->streams.push_back(s);
        }

        log_path = j["logs_path"];

        auto opencv_model_set = j["opencv_model_set"];
        opencv_model_history = opencv_model_set["history"];
        opencv_model_var_threshold = opencv_model_set["var_threshold"];
        opencv_model_detect_shadows = opencv_model_set["detect_shadows"];

        auto smtp = j["smtp"];
        smtp_server = smtp["server"];
        smtp_port = smtp["port"];
        smtp_username = smtp["username"];
    }

    int Config::getHowManyStreams() const {
        return streams.size();
    }

    std::string Config::getStreamUrl(int index) const {
        return streams[index].url;
    }
}