#pragma once

#include <string>

class MarketData
{
public:
    std::string symbol;
    double price;
    double changePercent;

    MarketData();

    void displayMarketInfo() const;
};