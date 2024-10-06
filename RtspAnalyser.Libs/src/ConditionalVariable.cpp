#include <atomic>
#include <memory>

#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"

using namespace Nico::RtspAnalyser::Libs;

ConditionalVariable::ConditionalVariable() :
    status(false)
{
}

void ConditionalVariable::notify()
{
    std::lock_guard<std::mutex> lock(this->lock);
    status = true;
    cond.notify_all();
}

void ConditionalVariable::wait()
{
    std::unique_lock<std::mutex> lock(this->lock);
    cond.wait(lock, [this] { return status; });
    status = false;
}

