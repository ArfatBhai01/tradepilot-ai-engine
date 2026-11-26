#include "SignalData.h"
#include <iostream>
#include <iomanip>

SignalData::SignalData()
{
    symbol = "BTC/USDT";
    type = SignalType::NONE;

    entryPrice = 0.0;
    stopLoss = 0.0;
    takeProfit = 0.0;
    confidence = 0.0;
}

bool SignalData::isValid() const
{
    if (symbol.empty())
        return false;

    if (entryPrice <= 0 || stopLoss <= 0 || takeProfit <= 0)
        return false;

    if (confidence < 0 || confidence > 100)
        return false;

    if (type == SignalType::BUY)
    {
        return stopLoss < entryPrice &&
            takeProfit > entryPrice;
    }

    if (type == SignalType::SELL)
    {
        return stopLoss > entryPrice &&
            takeProfit < entryPrice;
    }

    return false;
}

void SignalData::display() const
{
    std::cout << "\n========== TRADING SIGNAL ==========\n";
    std::cout << "Symbol: " << symbol << "\n";

    if (type == SignalType::BUY)
        std::cout << "Type: BUY\n";
    else if (type == SignalType::SELL)
        std::cout << "Type: SELL\n";
    else
        std::cout << "Type: NONE\n";

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Entry: $" << entryPrice << "\n";
    std::cout << "Stop Loss: $" << stopLoss << "\n";
    std::cout << "Take Profit: $" << takeProfit << "\n";
    std::cout << "Confidence: " << confidence << "%\n";

    std::cout << "Validation: "
        << (isValid() ? "VALID" : "INVALID")
        << "\n";

    std::cout << "==================================\n";
}