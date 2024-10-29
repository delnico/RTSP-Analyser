#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include <atomic>
#include <deque>
#include <chrono>

#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Libs/Spinlock.h"

#include "Nico/RtspAnalyser/Libs/Logger.h"

using namespace Nico::RtspAnalyser::Libs;

Logger * Logger::main_logger = nullptr;

void Logger::log_main(const std::string & message)
{
    if(main_logger != nullptr) {
        main_logger->log(message);
    }
    else {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string str_now = std::ctime(&now);
        str_now[str_now.size()-1] = '\0';
        std::cerr << str_now << " LOGGER : No main logger available \t error message : " << message << std::endl;
        std::cerr.flush();
    }
}

Logger::Logger(const std::string & filename) :
    slock_logs(),
    cond(),
    isEnabled(false),
    thread(),
    filename(filename),
    file()
{
    if(main_logger == nullptr) {
        main_logger = this;
    }

    try {
        file.open(filename, std::ios::out | std::ios::app);
    }
    catch(const std::exception & e) {
        std::cerr << "LOGGER : Failed to open file: " << e.what() << std::endl;
    }
}

Logger::~Logger()
{
    stop();
}

void Logger::start()
{
    if(! isEnabled.load()) {
        isEnabled.store(true);
        thread = std::thread(&Logger::run, this);
    }
}

void Logger::stop()
{
    if(isEnabled.load()) {
        isEnabled.store(false);
        cond.notify();
        thread.join();
        file.close();
    }
}

void Logger::log(const std::string & message)
{
    {
        std::lock_guard<Nico::RtspAnalyser::Libs::Spinlock> lock(slock_logs);
        logs.push_back(message);
    }
    cond.notify();
}

void Logger::run()
{
    std::string message;
    std::string str_now;
    while(isEnabled.load()) {
        cond.wait();
        {
            std::lock_guard<Nico::RtspAnalyser::Libs::Spinlock> lock(slock_logs);
            if(! logs.empty()) {
                message = logs.front();
                logs.pop_front();
            }
        }
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        str_now = std::ctime(&now);
        str_now[str_now.size()-1] = '\0';
        file << str_now << " : " << message << std::endl;
        file.flush();
    }
}
