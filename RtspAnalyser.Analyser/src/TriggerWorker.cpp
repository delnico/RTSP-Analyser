
#include <thread>
#include <atomic>
#include <deque>
#include <chrono>
#include <mutex>
#include <format>
#include <vector>

#include <fmt/chrono.h>
#include <curl/curl.h>

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
        username(username),
        curl_smtp_url("smtp://" + server_url + ":" + std::to_string(server_port))
    {}

    TriggerWorker::~TriggerWorker() {
        stop();
    }

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
            cv::Mat image = event.getPreviewImage();
            int score = event.getScore();
            int64_t  start_time = event.getStartTimestamp();
            int stream_id = event.getStreamId();
            sendEmail(score, image, start_time, stream_id);
        }
    }

    bool TriggerWorker::sendEmail(
        int score,
        const cv::Mat & image,
        int64_t start_time,
        int stream_id,
        std::string attachment_filename
    ) const
    {
        std::vector<uchar> buffer;
        std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, 85 };
        if(! cv::imencode(".jpg", image, buffer, params)) {
            Libs::Logger::log_main("TriggerWorker : Failed to encode image to JPEG");
            return false;
        }
        
        CURL *curl = curl_easy_init();
        if (!curl)
            return false;
        
        curl_easy_setopt(curl, CURLOPT_URL, curl_smtp_url.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, username.c_str());

        struct curl_slist *recipients = nullptr;
        recipients = curl_slist_append(recipients, username.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, ("To: " + username).c_str());
        headers = curl_slist_append(headers, ("From: " + username).c_str());

        std::chrono::nanoseconds duration(start_time);
        std::chrono::system_clock::time_point tp(
            std::chrono::duration_cast<std::chrono::system_clock::duration>(duration)
        );
        std::string formated = fmt::format("{:%d/%m/%Y %H:%M}", tp);
        std::string subject = "CAM " + std::to_string(stream_id) + " at " + formated;

        headers = curl_slist_append(headers, ("Subject: " + subject).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_mime *mime = curl_mime_init(curl);

        std::string body_text = "Score : " + std::to_string(score) + "\n";

        curl_mimepart *part_text = curl_mime_addpart(mime);
        curl_mime_data(part_text, body_text.c_str(), CURL_ZERO_TERMINATED);
        curl_mime_type(part_text, "text/plain; charset=utf-8");

        curl_mimepart *part_image = curl_mime_addpart(mime);

        curl_mime_data(part_image, reinterpret_cast<const char*>(buffer.data()), buffer.size());

        curl_mime_filename(part_image, attachment_filename.c_str());
        curl_mime_type(part_image, "image/jpeg");
        curl_mime_encoder(part_image, "base64");

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        CURLcode res = curl_easy_perform(curl);

        curl_slist_free_all(recipients);
        curl_slist_free_all(headers);
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        return res == CURLE_OK;
    }
}
