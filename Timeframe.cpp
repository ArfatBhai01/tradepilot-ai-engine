
#include "Timeframe.h"

std::string TimeframeManager::toString(Timeframe timeframe)
{
    switch (timeframe)
    {
    case Timeframe::M1:
        return "1m";

    case Timeframe::M5:
        return "5m";

    case Timeframe::M15:
        return "15m";

    case Timeframe::H1:
        return "1h";

    case Timeframe::H4:
        return "4h";

    case Timeframe::D1:
        return "1d";

    default:
        return "UNKNOWN";
    }
}

std::uint64_t TimeframeManager::toSeconds(Timeframe timeframe)
{
    switch (timeframe)
    {
    case Timeframe::M1:
        return 60;

    case Timeframe::M5:
        return 300;

    case Timeframe::M15:
        return 900;

    case Timeframe::H1:
        return 3600;

    case Timeframe::H4:
        return 14400;

    case Timeframe::D1:
        return 86400;

    default:
        return 0;
    }
}

bool TimeframeManager::isValid(Timeframe timeframe)
{
    return toSeconds(timeframe) > 0;
}
