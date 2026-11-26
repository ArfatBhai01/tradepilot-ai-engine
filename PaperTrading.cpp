#include "PaperTrading.h"

namespace TradePilot {

    PaperTradingAccount::PaperTradingAccount(double startingBalance) {
        this->startingBalance = startingBalance;
        this->balance = startingBalance;
    }

    double PaperTradingAccount::getBalance() const {
        return balance;
    }

    double PaperTradingAccount::getStartingBalance() const {
        return startingBalance;
    }

    double PaperTradingAccount::getTotalPnL() const {
        double total = 0.0;
        for (size_t i = 0; i < closedTrades.size(); i++) {
            total += closedTrades[i].pnl;
        }
        return total;
    }

    int PaperTradingAccount::getOpenCount() const {
        return (int)openPositions.size();
    }

    int PaperTradingAccount::getClosedCount() const {
        return (int)closedTrades.size();
    }

    bool PaperTradingAccount::openPosition(const std::string& symbol, const std::string& side, double entryPrice, double quantity, double stopLoss, double takeProfit) {
        double cost = entryPrice * quantity;
        if (cost > balance || cost <= 0) return false;
        OpenPosition pos;
        pos.symbol = symbol;
        pos.side = side;
        pos.entryPrice = entryPrice;
        pos.quantity = quantity;
        pos.stopLoss = stopLoss;
        pos.takeProfit = takeProfit;
        openPositions.push_back(pos);
        balance -= cost;
        return true;
    }

    bool PaperTradingAccount::closePosition(int index, double exitPrice) {
        if (index < 0 || index >= (int)openPositions.size()) return false;
        OpenPosition pos = openPositions[index];
        double pnl = 0.0;
        if (pos.side == "BUY") pnl = (exitPrice - pos.entryPrice) * pos.quantity;
        else pnl = (pos.entryPrice - exitPrice) * pos.quantity;
        double cost = pos.entryPrice * pos.quantity;
        balance += cost + pnl;
        ClosedTrade trade;
        trade.symbol = pos.symbol;
        trade.side = pos.side;
        trade.entryPrice = pos.entryPrice;
        trade.exitPrice = exitPrice;
        trade.pnl = pnl;
        closedTrades.push_back(trade);
        while ((int)closedTrades.size() > 100) {
            closedTrades.erase(closedTrades.begin());
        }
        openPositions.erase(openPositions.begin() + index);
        return true;
    }

    std::vector<OpenPosition> PaperTradingAccount::getOpenPositions() const {
        return openPositions;
    }

}