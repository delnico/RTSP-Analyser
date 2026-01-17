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
            std::vector<struct Stream> streams;
            std::string log_path;

            int opencv_model_history;
            int opencv_model_var_threshold;
            bool opencv_model_detect_shadows;
            int opencv_model_erode;
            int opencv_model_dilate;
            int opencv_model_gaussian_size;
            int opencv_model_gaussian_sigma;
            int opencv_model_min_area;

            
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
        }
        else if constexpr (std::is_same<T, int>::value)
        {
            if(key == "nvr_port") return nvr_port;
            if(key == "opencv_model_history") return opencv_model_history;
            if(key == "opencv_model_var_threshold") return opencv_model_var_threshold;
            if(key == "opencv_model_erode") return opencv_model_erode;
            if(key == "opencv_model_dilate") return opencv_model_dilate;
            if(key == "opencv_model_gaussian_size") return opencv_model_gaussian_size;
            if(key == "opencv_model_gaussian_sigma") return opencv_model_gaussian_sigma;
            if(key == "opencv_model_min_area") return opencv_model_min_area;
        }
        else if constexpr (std::is_same<T, bool>::value)
        {
            if(key == "opencv_model_detect_shadows") return opencv_model_detect_shadows;
        }

        throw std::runtime_error("Unknown key.");
    }
}



