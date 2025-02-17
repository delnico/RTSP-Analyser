#include <atomic>
#include <memory>

#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Libs/Spinlock.h"

using namespace DelNico::RtspAnalyser::Libs;

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

