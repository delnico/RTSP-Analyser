#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"

using namespace Nico::RtspAnalyser::Analyser;

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
