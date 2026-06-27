
#include <thread>
#include <atomic>
#include <deque>
#include <chrono>
#include <mutex>

#include <fmt/chrono.h>
#include <mailio/message.hpp>
#include <mailio/smtp.hpp>

#include "DelNico/RtspAnalyser/Libs/ConditionalVariable.h"
#include "DelNico/RtspAnalyser/Libs/Spinlock.h"
#include "DelNico/RtspAnalyser/Libs/Logger.h"
#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"
#include "DelNico/RtspAnalyser/Analyser/TriggerWorker.h"

namespace DelNico::RtspAnalyser::Analyser {
    TriggerWorker::TriggerWorker(
        std::string server_url,
        int server_port,
        std::string username
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

            std::chrono::nanoseconds duration(event.getStartTimestamp());
            std::chrono::system_clock::time_point tp(
                std::chrono::duration_cast<std::chrono::system_clock::duration>(duration)
            );
            std::string formated = fmt::format("{:%d/%m/%Y %H:%M}", tp);

            msg.subject("CAM " + std::to_string(event.getStreamId()) + " at " + formated);
            msg.content("A human has been detected by the RTSP Analyser.");

            Libs::Logger::log_main("TriggerWorker : Attempting connection to " + server_url + ":" + std::to_string(server_port));
            try {
                mailio::smtp conn(server_url, server_port);
                conn.authenticate("", "", mailio::smtp::auth_method_t::NONE);
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
