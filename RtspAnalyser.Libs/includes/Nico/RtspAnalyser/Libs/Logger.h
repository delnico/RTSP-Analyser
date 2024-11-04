#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include <atomic>
#include <deque>

#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Libs/Spinlock.h"



namespace Nico::RtspAnalyser::Libs {
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

            Nico::RtspAnalyser::Libs::Spinlock slock_logs;
            Nico::RtspAnalyser::Libs::ConditionalVariable cond;
            std::deque<std::string> logs;

            std::atomic<bool> isEnabled;
            std::thread thread;

            std::string filename;
            std::ofstream file;
    };
}

