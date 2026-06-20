
#include <thread>
#include <atomic>
#include <deque>
#include <chrono>
#include <mutex>

#include <mailio/message.hpp>
#include <mailio/smtp.hpp>

#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Libs/Spinlock.h"
#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"
#include "DelNico/RtspAnalyser/Analyser/TriggerWorker.h"

namespace DelNico::RtspAnalyser::Analyser {
    TriggerWorker::TriggerWorker(
        const std::string & server_url,
        int server_port,
        const std::string & username
    ) :
        thread(),
        isEnabled(false),
        slock_events(),
        events(),
        server_url(server_url),
        server_port(server_port),
        username(username)
    {}

    TriggerWorker::~TriggerWorker() {}

    void TriggerWorker::start() {
        if(! isEnabled.load()) {
            isEnabled.store(true);
            thread = std::thread(&TriggerWorker::run, this);
        }
    }

    void TriggerWorker::stop() {
        if(isEnabled.load()) {
            cv_wakeup.notify();
            isEnabled.store(false);
            thread.join();
        }
    }

    void TriggerWorker::addEvent(const Motion::MotionEvent & event) {
        std::lock_guard<Libs::Spinlock> lock(slock_events);
        events.push_back(event);
        cv_wakeup.notify();
    }

    void TriggerWorker::run() {
        while(isEnabled.load()) {
            cv_wakeup.wait();
            if(! isEnabled.load())
                break;
            Motion::MotionEvent event;
            {
                std::lock_guard<Libs::Spinlock> lock(slock_events);
                if(events.empty())
                    continue;
                event = events.front();
                events.pop_front();
            }
            mailio::message msg;
            msg.content_type(mailio::mime::media_type_t::TEXT, "plain", "utf-8");

            msg.from(mailio::mail_address(username, username));
            msg.add_recipient(mailio::mail_address(username, username));
            msg.subject("Human detected");
            msg.content("A human has been detected by the RTSP Analyser.");

            try {
                mailio::smtp conn(server_url, server_port);
                conn.submit(msg);
            }
            catch(mailio::smtp_error& exc) {
                Libs::Logger::log_main("TriggerWorker : SMTP error: " + std::string(exc.what()));
            }
            catch(mailio::dialog_error& exc) {
                Libs::Logger::log_main("TriggerWorker : Dialog error: " + std::string(exc.what()));
            }
            catch(std::exception& exc) {
                Libs::Logger::log_main("TriggerWorker : Exception: " + std::string(exc.what()));
            }
        }
    }

}
