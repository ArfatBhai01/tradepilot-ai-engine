
#pragma once

#include <string>
#include <cstdint>

enum class Timeframe
{
    M1,
    M5,
    M15,
    H1,
    H4,
    D1
};

class TimeframeManager
{
public:
    static std::string toString(Timeframe timeframe);

    static std::uint64_t toSeconds(Timeframe timeframe);

    static bool isValid(Timeframe timeframe);
};

