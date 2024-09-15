#pragma once

#include <string>
#include <list>

namespace Nico {
    namespace RtspAnalyser {
        namespace Libs {
            struct Stream {
                std::string url;
                std::string codec;
            };

            class Config {
                public:
                    Config();
                    Config(const std::string& file);
                    Config(const Config& orig) = delete;
                    Config& operator=(const Config& orig) = delete;
                    int getHowManyStreams() const;
                    std::string getStreamUrl(int index) const;
                    ~Config();
                private:
                    std::string nvr_ip;
                    int nvr_port;
                    std::string nvr_user;
                    std::string nvr_password;
                    std::string nvr_protocol;
                    std::list<struct Stream> streams;
            };
        }
    }
}
