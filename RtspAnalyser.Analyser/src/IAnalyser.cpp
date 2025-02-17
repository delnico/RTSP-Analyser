#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"

using namespace DelNico::RtspAnalyser::Analyser;

IAnalyser::~IAnalyser()
{
}

void IAnalyser::notify()
{
}

void IAnalyser::wait()
{
}

bool IAnalyser::operator==(const IAnalyser & other) const
{
    return &other == this;
}
