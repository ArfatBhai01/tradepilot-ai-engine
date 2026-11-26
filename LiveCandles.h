#pragma once
#include <string>
#include <vector>

namespace TradePilot {
    // Get full OHLC candles from Binance
    // symbol = "BTCUSDT", limit = 15
    struct Candle {
        long long time;
        double open;
        double high;
        double low;
        double close;
    };

    bool fetchLiveCloses(const std::string& symbol, int limit, std::vector<double>& outCloses);
    bool fetchLiveCandles(const std::string& symbol, int limit, std::vector<Candle>& outCandles);
}