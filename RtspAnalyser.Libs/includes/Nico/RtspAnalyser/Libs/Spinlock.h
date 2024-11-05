#pragma once

#include <atomic>



namespace Nico::RtspAnalyser::Libs {
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

