#include "MarketData.h"
#include <iostream>
#include <iomanip>

MarketData::MarketData()
{
    symbol = "BTC/USDT";
    price = 0.0;
    changePercent = 0.0;
}

void MarketData::displayMarketInfo() const
{
    std::cout << "\n========== MARKET DATA ==========\n";
    std::cout << "Symbol: " << symbol << "\n";
    std::cout << "Price: $" << std::fixed << std::setprecision(2) << price << "\n";
    std::cout << "Change: " << changePercent << "%\n";
    std::cout << "===============================\n";
}