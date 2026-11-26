#include "LiveMarketData.h"
#include <string>

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

    bool fetchLivePrice(const std::string& symbol, double& outPrice) {
        std::string url = "https://data-api.binance.vision/api/v3/ticker/price?symbol=" + symbol;
        std::string response;

#ifdef _WIN32
        HINTERNET hInternet = InternetOpenA("TradePilotAI", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) return false;

        HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
        if (!hUrl) {
            InternetCloseHandle(hInternet);
            return false;
        }

        char buffer[4096];
        DWORD bytesRead = 0;
        while (InternetReadFile(hUrl, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }

        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
#else
        CURL* curl = curl_easy_init();
        if (!curl) return false;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK) return false;
#endif

        std::string key = "\"price\":\"";
        size_t pos = response.find(key);
        if (pos == std::string::npos) return false;
        pos += key.length();
        size_t end = response.find("\"", pos);
        if (end == std::string::npos) return false;

        try {
            outPrice = std::stod(response.substr(pos, end - pos));
        }
        catch (...) {
            return false;
        }
        return outPrice > 0;
    }

}