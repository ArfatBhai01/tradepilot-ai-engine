#include "SignalServerSender.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <curl/curl.h>

static size_t WriteResponse(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* response = static_cast<std::string*>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}
#endif

namespace TradePilot {

    bool sendSignalToServer(const std::string& serverUrl,
        const std::string& secret,
        const std::string& symbol,
        const std::string& side,
        double entry,
        double stopLoss,
        double takeProfit,
        double confidence,
        double rsi) {
        // JSON body banao (2 decimal places)
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "{\"secret\":\"" << secret << "\","
            << "\"symbol\":\"" << symbol << "\","
            << "\"side\":\"" << side << "\","
            << "\"entry\":" << entry << ","
            << "\"stopLoss\":" << stopLoss << ","
            << "\"takeProfit\":" << takeProfit << ","
            << "\"confidence\":" << confidence << ","
            << "\"rsi\":" << rsi << "}";
        std::string body = ss.str();

#ifdef _WIN32
        // TelegramSender wala hi tareeka: Connect -> OpenRequest -> Send
        HINTERNET hInternet = InternetOpenA("TradePilotAI", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) return false;

        HINTERNET hConnect = InternetConnectA(hInternet, "arfuu0.pythonanywhere.com",
            INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect) { InternetCloseHandle(hInternet); return false; }

        HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", "/ingestSignal",
            NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
        if (!hRequest) { InternetCloseHandle(hConnect); InternetCloseHandle(hInternet); return false; }

        const char* headers = "Content-Type: application/json\r\n";
        BOOL sent = HttpSendRequestA(hRequest, headers, (DWORD)strlen(headers),
            (LPVOID)body.c_str(), (DWORD)body.size());

        bool ok = false;
        if (sent) {
            char buffer[512];
            DWORD read = 0;
            std::string response;
            while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &read) && read > 0) {
                buffer[read] = '\0';
                response += buffer;
            }
            // Server "ok": true bhejta hai jab signal save ho jaye
            ok = (response.find("\"ok\"") != std::string::npos);
        }

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return ok;
#else
        std::string fullUrl = serverUrl;
        if (fullUrl.empty()) fullUrl = "https://arfuu0.pythonanywhere.com";
        if (!fullUrl.empty() && fullUrl.back() == '/') fullUrl.pop_back();
        fullUrl += "/ingestSignal";

        CURL* curl = curl_easy_init();
        if (!curl) return false;

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.size());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) return false;
        // Server "ok": true bhejta hai jab signal save ho jaye
        return (response.find("\"ok\"") != std::string::npos);
#endif
    }

}