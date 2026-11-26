#pragma once
#include <string>

namespace TradePilot {

    enum class TradeDecision { BUY, SELL, NONE };

    struct StrategyResult {
        TradeDecision decision;
        double confidence;
        std::string reason;
    };

    // price = aaj ka price, ema = EMA value, rsi = RSI value
    // support / resistance = pichle step wale levels
    StrategyResult evaluateStrategy(double price, double ema, double rsi, double support, double resistance);

    std::string decisionToString(TradeDecision decision);

}
