
#include <thread>
#include <atomic>
#include <deque>
#include <chrono>
#include <mutex>

#include <mailio/message.hpp>
#include <mailio/smtp.hpp>

#include "DelNico/RtspAnalyser/Libs/Spinlock.h"
#include "DelNico/RtspAnalyser/Motion/MotionEvent.h"
#include "DelNico/RtspAnalyser/Analyser/TriggerWorker.h"

namespace DelNico::RtspAnalyser::Motion {
    TriggerWorker::TriggerWorker(
        const std::string & server_url,
        const std::string & username,
        const std::string & password
    ) :
        thread(),
        isEnabled(false),
        slock_events(),
        events(),
        server_url(server_url),
        username(username),
        password(password)
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
            isEnabled.store(false);
            thread.join();
        }
    }

    void TriggerWorker::addEvent(const MotionEvent & event) {
        std::lock_guard<Libs::Spinlock> lock(slock_events);
        events.push_back(event);
    }

    void TriggerWorker::run() {
        while(isEnabled.load()) {
            if(events.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            MotionEvent event;
            {
                std::lock_guard<Libs::Spinlock> lock(slock_events);
                event = events.front();
                events.pop_front();
            }
            mailio::message msg;
            msg.content_type(mailio::mime::media_type_t::TEXT, "plain", "utf-8");

            msg.from(mailio::mail_address(username));
            msg.add_recipient(mailio::mail_address(username));
            msg.subject("Human detected");
            msg.content("A human has been detected by the RTSP Analyser.");

            try {
                mailio::smtp conn(server_url, 587);
                conn.submit(msg);
            }
            catch(mailio::smtp_error& exc) {
                // Libs::Logger::log_main("TriggerWorker : SMTP error: " + std::string(exc.what()));
            }
            catch(mailio::dialog_error& exc) {

            }
            catch(std::exception& exc) {
                // Libs::Logger::log_main("TriggerWorker : Exception: " + std::string(exc.what()));
            }
        }
    }

}
