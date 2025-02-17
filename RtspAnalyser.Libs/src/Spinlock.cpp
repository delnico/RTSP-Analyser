#include <atomic>

#include "DelNico/RtspAnalyser/Libs/Spinlock.h"

using namespace DelNico::RtspAnalyser::Libs;

Spinlock::Spinlock() :
    flag(ATOMIC_FLAG_INIT)
{
}

Spinlock::Spinlock(const Spinlock & s)
{
    flag.test_and_set(s.getValue() ? std::memory_order_acquire : std::memory_order_release);
}

void Spinlock::lock()
{
    while(flag.test_and_set(std::memory_order_acquire));
}

void Spinlock::unlock()
{
    flag.clear(std::memory_order_release);
}

bool Spinlock::getValue() const
{
    return flag.test();
}
