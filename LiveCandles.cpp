#include "LiveCandles.h"

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <curl/curl.h>
#endif

#include <cctype>
#include <string>
#include <vector>

namespace TradePilot {

    // Binance klines URL se poora response lao (Windows = WinINet, Linux = curl)
#ifdef _WIN32
    static std::string fetchUrlWin(const std::string& url) {
        std::string out;
        HINTERNET hNet = InternetOpenA("TradePilotAI", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if (!hNet) return out;
        HINTERNET hUrl = InternetOpenUrlA(hNet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hUrl) { InternetCloseHandle(hNet); return out; }
        char buf[4096];
        DWORD read = 0;
        while (InternetReadFile(hUrl, buf, sizeof(buf), &read) && read > 0) {
            out.append(buf, read);
        }
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hNet);
        return out;
    }
#else
    static size_t WriteResponse(void* contents, size_t size, size_t nmemb, void* userp) {
        std::string* s = static_cast<std::string*>(userp);
        s->append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb;
    }
    static std::string fetchUrlCurl(const std::string& url) {
        std::string out;
        CURL* curl = curl_easy_init();
        if (!curl) return out;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        return out;
    }
#endif

    static std::string fetchUrl(const std::string& url) {
#ifdef _WIN32
        return fetchUrlWin(url);
#else
        return fetchUrlCurl(url);
#endif
    }

    // Binance klines JSON se full OHLC candles nikalo
    // Har candle: [time, "open", "high", "low", "close", "volume", ...]
    static bool parseCandles(const std::string& json, std::vector<Candle>& out) {
        out.clear();
        size_t i = 0;
        while (i < json.size()) {
            size_t b = json.find('[', i);
            if (b == std::string::npos) break;
            size_t j = b + 1;
            while (j < json.size() && std::isspace((unsigned char)json[j])) j++;
            if (j >= json.size() || !std::isdigit((unsigned char)json[j])) { i = b + 1; continue; }
            long long t = 0;
            while (j < json.size() && std::isdigit((unsigned char)json[j])) { t = t * 10 + (json[j] - '0'); j++; }
            double vals[4];
            int got = 0;
            while (got < 4) {
                size_t q1 = json.find('"', j);
                if (q1 == std::string::npos) break;
                size_t q2 = json.find('"', q1 + 1);
                if (q2 == std::string::npos) break;
                try { vals[got] = std::stod(json.substr(q1 + 1, q2 - q1 - 1)); }
                catch (...) { break; }
                got++;
                j = q2 + 1;
            }
            if (got == 4) {
                Candle c;
                c.time = t;
                c.open = vals[0];
                c.high = vals[1];
                c.low = vals[2];
                c.close = vals[3];
                out.push_back(c);
            }
            i = j;
        }
        return out.size() >= 5;
    }

    bool fetchLiveCloses(const std::string& symbol, int limit, std::vector<double>& outCloses) {
        outCloses.clear();
        std::string url = "https://api.binance.com/api/v3/klines?symbol=" + symbol + "&interval=1h&limit=" + std::to_string(limit);
        std::string body = fetchUrl(url);
        if (body.empty()) return false;
        std::vector<Candle> candles;
        if (!parseCandles(body, candles)) return false;
        for (size_t k = 0; k < candles.size(); k++) outCloses.push_back(candles[k].close);
        return outCloses.size() >= 5;
    }

    bool fetchLiveCandles(const std::string& symbol, int limit, std::vector<Candle>& outCandles) {
        outCandles.clear();
        std::string url = "https://api.binance.com/api/v3/klines?symbol=" + symbol + "&interval=1h&limit=" + std::to_string(limit);
        std::string body = fetchUrl(url);
        if (body.empty()) return false;
        return parseCandles(body, outCandles);
    }

} // namespace TradePilot