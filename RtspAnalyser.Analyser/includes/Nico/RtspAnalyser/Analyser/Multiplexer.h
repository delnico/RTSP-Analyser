#pragma once

#include <atomic>
#include <deque>
#include <list>
#include <thread>

#include "Nico/RtspAnalyser/Analyser/IAnalyser.h"
#include "Nico/RtspAnalyser/Analyser/OutputStream.h"
#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"

namespace Nico {
    namespace RtspAnalyser {
        namespace Analyser {
            class Multiplexer : public IAnalyser {
                public:
                    Multiplexer() = delete;
                    Multiplexer(
                        std::deque<cv::Mat> & input_frames
                    );
                    ~Multiplexer();

                    void subscribe(OutputStream * output_client);
                    void unsubscribe(OutputStream * output_client);

                    void start();
                    void stop();

                    void notify() override;

                private:
                    std::atomic<bool> isEnabled;
                    std::thread thread;
                    Libs::ConditionalVariable input_cond;

                    std::deque<cv::Mat> & input_frames;

                    std::list<OutputStream *> output_clients;
                    int64_t frame_count;

                    void run();
                    void multiplex(cv::Mat frame);
            };
        }
    }
}
