#include <atomic>
#include <memory>

#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Libs/Spinlock.h"

using namespace Nico::RtspAnalyser::Libs;

ConditionalVariable::ConditionalVariable() :
    status(false)
{
}

ConditionalVariable::ConditionalVariable(Spinlock & spinlock) :
    spinlock(spinlock),
    status(false)
{
}

void ConditionalVariable::notify()
{
    std::unique_lock<Spinlock> lock(this->spinlock);
    status = true;
    cond.notify_all();
}

void ConditionalVariable::wait()
{
    std::unique_lock<Spinlock> lock(this->spinlock);
    cond.wait(lock, [this] { return status.load(); });
    status = false;
}

