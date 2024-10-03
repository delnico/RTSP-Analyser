
#pragma once

// Analyser interface

namespace Nico {
    namespace RtspAnalyser {
        namespace Analyser {
            class IAnalyser {
                public:
                    virtual ~IAnalyser() = default;
                    virtual void notify();

                    virtual bool operator==(const IAnalyser & other) const;
            };
        }
    }
}
