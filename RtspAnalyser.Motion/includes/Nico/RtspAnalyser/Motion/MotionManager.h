#pragma once

#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include "Nico/RtspAnalyser/Analyser/Multiplexer.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Motion/MotionEvent.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Motion {
            class MotionManager {
            public:
                MotionManager() = delete;
                MotionManager(
                    Nico::RtspAnalyser::Libs::ConditionalVariable & cond_events,
                    std::chrono::seconds guard_time_new_event,
                    Nico::RtspAnalyser::Analyser::Multiplexer * multiplexer
                );
                ~MotionManager();

                void start();
                void stop();

                void notify();

            private:
                void run();

                std::thread thread;
                std::atomic<bool> isEnabled;
                std::deque<MotionEvent> events;
                Nico::RtspAnalyser::Libs::ConditionalVariable & cond_events;
                std::chrono::seconds guard_time_new_event;
                Nico::RtspAnalyser::Analyser::Multiplexer * multiplexer;
            };
        }
    }
}