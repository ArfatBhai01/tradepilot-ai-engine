#include "TelegramSender.h"
#include <string>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <curl/curl.h>

static size_t DiscardResponse(char* /*ptr*/, size_t size, size_t nmemb, void* /*userdata*/) {
    return size * nmemb;
}
#endif

namespace TradePilot {

    static std::string escapeJson(const std::string& s) {
        std::string out;
        for (char c : s) {
            if (c == '"') out += "\\\"";
            else if (c == '\\') out += "\\\\";
            else if (c == '\n') out += "\\n";
            else out += c;
        }
        return out;
    }

    bool sendTelegramSignal(const std::string& botToken, const std::string& chatId, const std::string& text) {
        std::string path = "/bot" + botToken + "/sendMessage";
        std::string body = "{\"chat_id\":\"" + chatId + "\",\"parse_mode\":\"HTML\",\"text\":\"" + escapeJson(text) + "\"}";

#ifdef _WIN32
        HINTERNET hInternet = InternetOpenA("TradePilotAI", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) return false;
        HINTERNET hConnect = InternetConnectA(hInternet, "api.telegram.org", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect) { InternetCloseHandle(hInternet); return false; }
        HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path.c_str(), NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
        if (!hRequest) { InternetCloseHandle(hConnect); InternetCloseHandle(hInternet); return false; }

        const char* headers = "Content-Type: application/json\r\n";
        BOOL ok = HttpSendRequestA(hRequest, headers, (DWORD)strlen(headers), (LPVOID)body.c_str(), (DWORD)body.size());

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return ok == TRUE;
#else
        std::string fullUrl = "https://api.telegram.org" + path;

        CURL* curl = curl_easy_init();
        if (!curl) return false;

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.size());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DiscardResponse);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return res == CURLE_OK;
#endif
    }

}