#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include <atomic>
#include <deque>

#include "Nico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "Nico/RtspAnalyser/Libs/Spinlock.h"

#include "Nico/RtspAnalyser/Libs/Logger.h"

using namespace Nico::RtspAnalyser::Libs;

Logger::Logger(const std::string & filename) :
    slock_logs(),
    cond(slock_logs),
    isEnabled(false),
    thread(),
    filename(filename),
    file()
{
    try {
        file.open(filename, std::ios::out | std::ios::app);
    }
    catch(const std::exception & e) {
        std::cerr << "Failed to open file: " << e.what() << std::endl;
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
    if(thread.joinable()) {
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
    while(isEnabled.load()) {
        std::string message;
        {
            std::unique_lock<Nico::RtspAnalyser::Libs::Spinlock> lock(slock_logs);
            cond.wait();
            message = logs.front();
            logs.pop_front();
        }
        file << message << std::endl;
    }
}
