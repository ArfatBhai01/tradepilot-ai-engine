#include "RiskManager.h"
#include <cmath>

bool RiskManager::validateSignal(
    const SignalData& signal,
    std::string& reason)
{
    // Check symbol
    if (signal.symbol.empty())
    {
        reason = "Symbol is missing";
        return false;
    }

    // Check prices
    if (signal.entryPrice <= 0 ||
        signal.stopLoss <= 0 ||
        signal.takeProfit <= 0)
    {
        reason = "Invalid price values";
        return false;
    }

    // Check confidence
    if (signal.confidence < 60.0)
    {
        reason = "Confidence too low";
        return false;
    }

    // BUY validation
    if (signal.type == SignalType::BUY)
    {
        if (signal.stopLoss >= signal.entryPrice)
        {
            reason = "BUY Stop Loss must be below Entry";
            return false;
        }

        if (signal.takeProfit <= signal.entryPrice)
        {
            reason = "BUY Take Profit must be above Entry";
            return false;
        }
    }

    // SELL validation
    else if (signal.type == SignalType::SELL)
    {
        if (signal.stopLoss <= signal.entryPrice)
        {
            reason = "SELL Stop Loss must be above Entry";
            return false;
        }

        if (signal.takeProfit >= signal.entryPrice)
        {
            reason = "SELL Take Profit must be below Entry";
            return false;
        }
    }

    else
    {
        reason = "Unknown signal type";
        return false;
    }

    // Risk / Reward calculation
    double risk = std::abs(signal.entryPrice - signal.stopLoss);
    double reward = std::abs(signal.takeProfit - signal.entryPrice);

    if (risk <= 0)
    {
        reason = "Risk calculation invalid";
        return false;
    }

    double riskRewardRatio = reward / risk;

    if (riskRewardRatio < 1.2)
    {
        reason = "Risk reward ratio too low";
        return false;
    }

    reason = "Signal passed risk validation";
    return true;
}