#include "EngineBridge.h"
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

    bool sendHeartbeat(const std::string& fullUrl, const std::string& apiKey, int totalSignals) {
        std::string body = "{\"apiKey\":\"" + apiKey + "\",\"engineVersion\":\"1.0\",\"mode\":\"PAPER TRADING\",\"totalSignals\":" + std::to_string(totalSignals) + "}";

#ifdef _WIN32
        // https:// hatao, host aur path alag karo
        std::string url = fullUrl;
        if (url.rfind("https://", 0) == 0) url = url.substr(8);
        if (url.rfind("http://", 0) == 0) url = url.substr(7);
        size_t slash = url.find('/');
        std::string host = (slash == std::string::npos) ? url : url.substr(0, slash);
        std::string path = (slash == std::string::npos) ? "/" : url.substr(slash);

        HINTERNET hInternet = InternetOpenA("TradePilotAI", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) return false;
        HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
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

} // namespace TradePilot