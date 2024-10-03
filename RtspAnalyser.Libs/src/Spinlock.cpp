#include <atomic>

#include "Nico/RtspAnalyser/Libs/Spinlock.h"

using namespace Nico::RtspAnalyser::Libs;

Spinlock::Spinlock() :
    flag(ATOMIC_FLAG_INIT)
{
}

void Spinlock::lock()
{
    while(! flag.test_and_set(std::memory_order_acquire));
}

void Spinlock::unlock()
{
    flag.clear(std::memory_order_release);
}
