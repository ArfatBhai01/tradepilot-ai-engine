#pragma once

#include <string>

enum class SignalType
{
    BUY,
    SELL,
    NONE
};

class SignalData
{
public:
    std::string symbol;
    SignalType type;

    double entryPrice;
    double stopLoss;
    double takeProfit;
    double confidence;

    SignalData();

    bool isValid() const;
    void display() const;
};