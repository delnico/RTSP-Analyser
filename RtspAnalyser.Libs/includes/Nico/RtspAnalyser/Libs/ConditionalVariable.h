#pragma once

#include <condition_variable>
#include <mutex>
#include <atomic>

#include "Nico/RtspAnalyser/Libs/Spinlock.h"

namespace Nico
{
    namespace RtspAnalyser
    {
        namespace Libs
        {
            class ConditionalVariable
            {
                public:
                    ConditionalVariable();
                    ConditionalVariable(Spinlock & spinlock);
                    ~ConditionalVariable() = default;
                    void notify();
                    void wait();
                private:
                    Nico::RtspAnalyser::Libs::Spinlock spinlock;
                    std::condition_variable_any cond;
                    std::atomic<bool> status;
            };
        }
    }
}
