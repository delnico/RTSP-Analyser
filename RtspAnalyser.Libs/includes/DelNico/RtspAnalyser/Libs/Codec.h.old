#pragma once

#include <iostream>



namespace DelNico::RtspAnalyser::Libs {
  enum class Codec {
        H264,
        H265
    };
    inline std::ostream& operator<<(std::ostream& os, const Codec& codec) {
        switch(codec) {
            case Codec::H264:
                os << "H264";
                break;
            case Codec::H265:
                os << "H265";
                break;
        }
        return os;
    }
}

