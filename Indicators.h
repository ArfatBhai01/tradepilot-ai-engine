
#pragma once

#include <vector>
#include <cstddef>

struct MACDResult
{
    double macdLine;
    double signalLine;
    double histogram;
    bool valid;
};

class Indicators
{
public:
    static double calculateSMA(
        const std::vector<double>& prices,
        std::size_t period
    );

    static double calculateEMA(
        const std::vector<double>& prices,
        std::size_t period
    );

    static double calculateRSI(
        const std::vector<double>& prices,
        std::size_t period = 14
    );

    static MACDResult calculateMACD(
        const std::vector<double>& prices,
        std::size_t fastPeriod = 12,
        std::size_t slowPeriod = 26,
        std::size_t signalPeriod = 9
    );
};

