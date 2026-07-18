#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <memory>

#include <opencv2/opencv.hpp>
#include <onnxruntime/onnxruntime_cxx_api.h>
#include <oneapi/tbb/concurrent_queue.h>

#include "DelNico/RtspAnalyser/Analyser/IAnalyser.h"
#include "DelNico/RtspAnalyser/Analyser/Streamer.h"
#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Motion/MotionManager.h"


namespace DelNico::RtspAnalyser::Analyser {
    class HumanDetector : public IAnalyser {
        public:
            HumanDetector() = delete;
            HumanDetector(
                oneapi::tbb::concurrent_queue<cv::Mat> & frames,
                Motion::MotionManager * motionManager,
                std::vector<cv::Rect> zones,
                float confidence_threshold,
                bool onCPU = true
            );
            ~HumanDetector() override;
            HumanDetector(const HumanDetector & other) = delete;
            HumanDetector & operator=(const HumanDetector & other) = delete;

            void setStreamer(Streamer * streamer, oneapi::tbb::concurrent_queue<cv::Mat> * human_detected_output);
            
            void start();
            void stop();
            void notify() override;

        private:
            Libs::ConditionalVariable cond;
            std::atomic<bool> isEnabled;
            std::thread thread;
            oneapi::tbb::concurrent_queue<cv::Mat> & frames;
            Motion::MotionManager * motionManager;
            std::vector<cv::Rect> zones;
            float confidence_threshold;
            Streamer * streamer;
            oneapi::tbb::concurrent_queue<cv::Mat> * human_detected_output;

            std::unique_ptr<Ort::Env> env;
            std::unique_ptr<Ort::Session> session;
            std::vector<const char*> input_names;
            std::vector<const char*> output_names;

            void run();
            std::tuple<bool, cv::Mat, float>  isHumanDetected(const cv::Mat & frame, const bool need_output = false) const;
            bool isHumanInsideZone(int x_center, int y_center) const;

            bool operator==(const HumanDetector & other) const;
    };
}