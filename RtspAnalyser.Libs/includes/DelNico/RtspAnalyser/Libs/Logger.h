#pragma once

#include <string>
#include <fstream>
#include <thread>
#include <atomic>
#include <deque>

#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Libs/Spinlock.h"


namespace DelNico::RtspAnalyser::Libs {
    class Logger {
        public:
            static Logger * main_logger;
            static void log_main(const std::string & message);


            Logger() = delete;
            Logger(const std::string & filename);
            ~Logger();

            void start();
            void stop();

            void log(const std::string & message);

        private:
            void run();

            Spinlock slock_logs;
            ConditionalVariable cond;
            std::deque<std::string> logs;

            std::atomic<bool> isEnabled;
            std::thread thread;

            std::string filename;
            std::ofstream file;
    };
}
