
#include "CandleHistory.h"

#include <iostream>
#include <utility>

CandleHistory::CandleHistory(const std::string& marketSymbol)
    : symbol(marketSymbol)
{
}

bool CandleHistory::addCandle(const CandleData& candle)
{
    if (symbol.empty() || !candle.isValid())
    {
        return false;
    }

    // Reject duplicate or older timestamps.
    if (!candles.empty())
    {
        const CandleData& latest = candles.back();

        if (candle.timestamp <= latest.timestamp)
        {
            return false;
        }
    }

    candles.push_back(candle);
    return true;
}

const CandleData* CandleHistory::getLatestCandle() const
{
    if (candles.empty())
    {
        return nullptr;
    }

    return &candles.back();
}

std::vector<CandleData> CandleHistory::getCandlesByTimeframe(
    Timeframe timeframe
) const
{
    std::vector<CandleData> result;

    for (const CandleData& candle : candles)
    {
        if (candle.timeframe == timeframe)
        {
            result.push_back(candle);
        }
    }

    return result;
}

std::size_t CandleHistory::getCount() const
{
    return candles.size();
}

void CandleHistory::displayHistory() const
{
    std::cout << "\n========== CANDLE HISTORY ==========\n";

    std::cout << "Symbol: " << symbol << "\n";
    std::cout << "Total Candles: " << candles.size() << "\n";

    for (const CandleData& candle : candles)
    {
        candle.display();
    }

    std::cout << "====================================\n";
}
