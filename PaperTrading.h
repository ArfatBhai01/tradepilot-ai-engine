#pragma once
#include <string>
#include <vector>

namespace TradePilot {

    struct OpenPosition {
        std::string symbol;
        std::string side;
        double entryPrice;
        double quantity;
        double stopLoss;
        double takeProfit;
    };

    struct ClosedTrade {
        std::string symbol;
        std::string side;
        double entryPrice;
        double exitPrice;
        double pnl;
    };

    class PaperTradingAccount {
    private:
        double startingBalance;
        double balance;
        std::vector<OpenPosition> openPositions;
        std::vector<ClosedTrade> closedTrades;

    public:
        PaperTradingAccount(double startingBalance = 10000.0);
        double getBalance() const;
        double getStartingBalance() const;
        double getTotalPnL() const;
        int getOpenCount() const;
        int getClosedCount() const;
        bool openPosition(const std::string& symbol, const std::string& side, double entryPrice, double quantity, double stopLoss, double takeProfit);
        bool closePosition(int index, double exitPrice);
        std::vector<OpenPosition> getOpenPositions() const;
    };

}
