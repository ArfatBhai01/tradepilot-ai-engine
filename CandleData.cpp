
#include "CandleData.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

CandleData::CandleData()
{
    timestamp = 0;

    open = 0.0;
    high = 0.0;
    low = 0.0;
    close = 0.0;
    volume = 0.0;

    timeframe = Timeframe::M1;
}

bool CandleData::isValid() const
{
    if (!std::isfinite(open) ||
        !std::isfinite(high) ||
        !std::isfinite(low) ||
        !std::isfinite(close) ||
        !std::isfinite(volume))
    {
        return false;
    }

    if (open <= 0 ||
        high <= 0 ||
        low <= 0 ||
        close <= 0 ||
        volume < 0)
    {
        return false;
    }

    double highestPrice = std::max(open, close);
    double lowestPrice = std::min(open, close);

    if (high < highestPrice)
        return false;

    if (low > lowestPrice)
        return false;

    if (high < low)
        return false;

    if (!TimeframeManager::isValid(timeframe))
        return false;

    return true;
}

void CandleData::display() const
{
    std::cout << "\n========== CANDLE DATA ==========\n";

    std::cout << "Timestamp: " << timestamp << "\n";
    std::cout << "Timeframe: "
        << TimeframeManager::toString(timeframe)
        << "\n";

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Open:   $" << open << "\n";
    std::cout << "High:   $" << high << "\n";
    std::cout << "Low:    $" << low << "\n";
    std::cout << "Close:  $" << close << "\n";
    std::cout << "Volume: " << volume << "\n";

    std::cout << "Validation: "
        << (isValid() ? "VALID" : "INVALID")
        << "\n";

    std::cout << "=================================\n";
}
