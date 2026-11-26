
#pragma once

#include <cstdint>
#include "Timeframe.h"

class CandleData
{
public:
    std::uint64_t timestamp;

    double open;
    double high;
    double low;
    double close;
    double volume;

    Timeframe timeframe;

    CandleData();

    bool isValid() const;

    void display() const;
};

