#pragma once

#include <atomic>

namespace Nico {
    namespace RtspAnalyser {
        namespace Libs {
            class Spinlock {
                public:
                    Spinlock();
                    void lock();
                    void unlock();

                private:
                    std::atomic_flag flag;
            };
        }
    }
}