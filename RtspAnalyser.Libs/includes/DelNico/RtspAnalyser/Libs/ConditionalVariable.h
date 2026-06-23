#pragma once

#include <condition_variable>
#include <atomic>
#include <mutex>

#include "DelNico/RtspAnalyser/Libs/Spinlock.h"


namespace DelNico::RtspAnalyser::Libs
{
    class ConditionalVariable
    {
        public:
            ConditionalVariable();
            // ConditionalVariable(Spinlock & spinlock);
            ~ConditionalVariable() = default;
            void notify();
            void wait();
        private:
            std::mutex spinlock;
            std::condition_variable_any cond;
            std::atomic<bool> status;
    };
}
