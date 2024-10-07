#pragma once

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Analyser {
            class MotionDetector : public IAnalyser {
                public:
                    MotionDetector();
                    ~MotionDetector();

                    void run();
                    void start();
                    void stop();

                private:
                    void notify() override;
                    void wait() override;

                    bool operator==(const MotionDetector & other) const;
            };
        }
    }
}