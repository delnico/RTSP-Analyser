#pragma once

#include <string>
#include <vector>
#include "DelNico/RtspAnalyser/Libs/Codec.h"
#include "DelNico/RtspAnalyser/Libs/Stream.h"



namespace DelNico::RtspAnalyser::Libs {
    class Config {
        public:
            Config() = delete;
            Config(const std::string& file);
            Config(const Config& orig) = delete;
            Config& operator=(const Config& orig) = delete;
            ~Config() = default;


            // use template to avoid code duplication
            // T can be a string, int, bool, ...
            template <typename T>
            T get(const std::string& key);


            int getHowManyStreams() const;
            std::string getStreamUrl(int index) const;
            Codec getStreamCodec(int index) const;

        private:
            std::string nvr_ip;
            int nvr_port;
            std::string nvr_user;
            std::string nvr_password;
            std::string nvr_protocol;
            std::string nvr_gstreamer_pipeline_params;
            std::vector<struct Stream> streams;
            std::string log_path;

            int opencv_model_history;
            int opencv_model_var_threshold;
            bool opencv_model_detect_shadows;

            std::string smtp_server;
            int smtp_port;
            std::string smtp_username;
    };

    template<typename T>
    T Config::get(const std::string& key)
    {
        if constexpr (std::is_same<T, std::string>::value)
        {
            if(key == "nvr_ip") return nvr_ip;
            if(key == "nvr_user") return nvr_user;
            if(key == "nvr_password") return nvr_password;
            if(key == "nvr_protocol") return nvr_protocol;
            if(key == "log_path") return log_path;
            if(key == "smtp_server") return smtp_server;
            if(key == "smtp_username") return smtp_username;
            if(key == "nvr_gstreamer_pipeline_params") return nvr_gstreamer_pipeline_params;
        }
        else if constexpr (std::is_same<T, int>::value)
        {
            if(key == "nvr_port") return nvr_port;
            if(key == "opencv_model_history") return opencv_model_history;
            if(key == "opencv_model_var_threshold") return opencv_model_var_threshold;
            if(key == "smtp_port") return smtp_port;
        }
        else if constexpr (std::is_same<T, bool>::value)
        {
            if(key == "opencv_model_detect_shadows") return opencv_model_detect_shadows;
        }

        throw std::runtime_error("Unknown key.");
    }
}
