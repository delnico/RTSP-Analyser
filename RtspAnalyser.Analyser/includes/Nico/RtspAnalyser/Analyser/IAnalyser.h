#pragma once

// Analyser abstract class

namespace Nico::RtspAnalyser::Analyser {
    class IAnalyser {
        public:
            virtual ~IAnalyser();
            virtual void notify();
            virtual void wait();

            virtual bool operator==(const IAnalyser & other) const;
    };
}
