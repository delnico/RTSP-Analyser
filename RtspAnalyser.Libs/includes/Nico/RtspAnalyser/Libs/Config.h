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

                private:
                    std::string nvr_ip;
                    int nvr_port;
                    std::string nvr_user;
                    std::string nvr_password;
                    std::string nvr_protocol;
                    std::vector<struct Stream> streams;
                    std::string log_file_path;
            };
        }
    }
}
