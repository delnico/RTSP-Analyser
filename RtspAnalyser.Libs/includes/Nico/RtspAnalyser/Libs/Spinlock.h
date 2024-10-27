#pragma once

#include <atomic>

namespace Nico {
    namespace RtspAnalyser {
        namespace Libs {
            class Spinlock {
                public:
                    Spinlock();
                    Spinlock(const Spinlock &);
                    void lock();
                    void unlock();

                    bool getValue() const;

                private:
                    std::atomic_flag flag;
            };
        }
    }
}