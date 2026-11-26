#include "Strategy.h"
#include <cmath>

namespace TradePilot {

    StrategyResult evaluateStrategy(double price, double ema, double rsi, double support, double resistance) {
        StrategyResult result;
        result.confidence = 50.0;

        if (price > ema && rsi < 70.0) {
            result.decision = TradeDecision::BUY;
            result.confidence = 70.0;
            result.reason = "Price EMA se upar hai aur RSI 70 se kam hai";
            if (support > 0 && std::abs(price - support) / support * 100.0 <= 2.0) {
                result.confidence = 75.0;
                result.reason += " + Support ke paas hai";
            }
        }
        else if (price < ema && rsi > 30.0) {
            result.decision = TradeDecision::SELL;
            result.confidence = 70.0;
            result.reason = "Price EMA se neeche hai aur RSI 30 se zyada hai";
            if (resistance > 0 && std::abs(price - resistance) / resistance * 100.0 <= 2.0) {
                result.confidence = 75.0;
                result.reason += " + Resistance ke paas hai";
            }
        }
        else {
            result.decision = TradeDecision::NONE;
            result.confidence = 50.0;
            result.reason = "Koi clear trend nahi hai, wait karo";
        }
        return result;
    }

    std::string decisionToString(TradeDecision decision) {
        if (decision == TradeDecision::BUY) return "BUY";
        if (decision == TradeDecision::SELL) return "SELL";
        return "NONE";
    }

}