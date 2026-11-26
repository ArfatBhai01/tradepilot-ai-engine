
#pragma once

#include <vector>
#include <string>
#include <cstddef>

#include "CandleData.h"

class CandleHistory
{
private:
    std::string symbol;
    std::vector<CandleData> candles;

public:
    explicit CandleHistory(const std::string& marketSymbol);

    bool addCandle(const CandleData& candle);

    const CandleData* getLatestCandle() const;

    std::vector<CandleData> getCandlesByTimeframe(
        Timeframe timeframe
    ) const;

    std::size_t getCount() const;

    void displayHistory() const;
};

