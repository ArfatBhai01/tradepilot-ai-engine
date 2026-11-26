#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include "MarketData.h"
#include "SignalData.h"
#include "SignalManager.h"
#include "RiskManager.h"
#include "Indicators.h"
#include "SupportResistance.h"
#include "Strategy.h"
#include "SignalHistory.h"
#include "PaperTrading.h"
#include "LiveMarketData.h"
#include "LiveCandles.h"
#include "TelegramSender.h"
#include <sstream>
#include <iomanip>
#include "SignalServerSender.h"
#include <cstdlib>

int main() {
    std::cout << "==============================" << std::endl;
    std::cout << "      TRADEPILOT AI" << std::endl;
    std::cout << "  MULTI-SIGNAL SYSTEM" << std::endl;
    std::cout << "==============================" << std::endl;

    double livePrice = 0;
    bool liveOk = TradePilot::fetchLivePrice("BTCUSDT", livePrice);
    double btcPrice = liveOk ? livePrice : 95000.00;
    if (liveOk) {
        std::cout << "Live Price: $" << livePrice << " (Binance)" << std::endl;
    }
    else {
        std::cout << "Live price nahi mila, demo price use ho raha hai" << std::endl;
    }

    MarketData btc;
    btc.symbol = "BTC/USDT";
    btc.price = btcPrice;
    btc.changePercent = 2.45;
    btc.displayMarketInfo();

    std::vector<double> closes;
    std::vector<double> demoCloses = { 93000, 93500, 94000, 93800, 94500, 95000, 94800, 95500, 95200, 96000, 95800, 96500, 96200, 97000, 95000 };
    bool candlesLive = TradePilot::fetchLiveCloses("BTCUSDT", 15, closes);
    if (candlesLive) {
        std::cout << "Live Candles: " << closes.size() << " (Binance 1h)" << std::endl;
    }
    else {
        closes = demoCloses;
        std::cout << "Live candles nahi mile, demo candles use ho rahe hain" << std::endl;
    }

    std::vector<double> lows, highs;
    for (double c : closes) { lows.push_back(c - 300); highs.push_back(c + 300); }

    double sma = Indicators::calculateSMA(closes, 14);
    double ema = Indicators::calculateEMA(closes, 14);
    double rsi = Indicators::calculateRSI(closes, 14);
    double support = TradePilot::calculateSupport(lows, 14);
    double resistance = TradePilot::calculateResistance(highs, 14);

    std::cout << "SMA 14: " << sma << std::endl;
    std::cout << "EMA 14: " << ema << std::endl;
    std::cout << "RSI 14: " << rsi << std::endl;
    std::cout << "Support: " << support << std::endl;
    std::cout << "Resistance: " << resistance << std::endl;

    TradePilot::StrategyResult strat = TradePilot::evaluateStrategy(btc.price, ema, rsi, support, resistance);
    std::cout << "Strategy: " << TradePilot::decisionToString(strat.decision) << " Confidence " << strat.confidence << "%" << std::endl;
    std::cout << "Reason: " << strat.reason << std::endl;
    const char* _botToken = std::getenv("8811675730:AAEZH1-5-iJsA8jmS4dOav9xsb73OSe_YLg");
    std::string botToken = _botToken ? _botToken : "";
    const char* _chatId = std::getenv("8606233084");
    std::string chatId = _chatId ? _chatId : "";
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "TradePilot AI Signal\nBTC/USDT: " << TradePilot::decisionToString(strat.decision) << "\nPrice: $" << btc.price << "\nConfidence: " << strat.confidence << "%\nRSI: " << rsi << "\nPaper trading only - profit ki guarantee nahi hai.";
    std::string signalText = ss.str();
    bool sent = TradePilot::sendTelegramSignal(botToken, chatId, signalText);
    const char* _serverSecret = std::getenv("BOT_SERVER_SECRET");
    std::string serverSecret = _serverSecret ? _serverSecret : "";
    // C++ signal ko server pe bhejo (Mini App mein dikhega)
    std::string side = TradePilot::decisionToString(strat.decision);
    double sigSL = (side == "BUY") ? btc.price - 1000 : btc.price + 1000;
    double sigTP = (side == "BUY") ? btc.price + 2000 : btc.price - 2000;
    bool serverSent = TradePilot::sendSignalToServer("https://arfuu0.pythonanywhere.com", serverSecret, "BTCUSDT", side, btc.price, sigSL, sigTP, strat.confidence, rsi);
    std::cout << "Server Signal Sent: " << (serverSent ? "Yes" : "No") << std::endl;

    TradePilot::SignalHistory history;
    history.addSignal("BTC/USDT", "BUY", btc.price, 72.5);
    std::cout << "History Count: " << history.getCount() << std::endl;

    TradePilot::PaperTradingAccount account(10000);
    std::cout << "Paper Balance Start: $" << account.getBalance() << std::endl;
    bool opened = account.openPosition("BTC/USDT", "BUY", btc.price, 0.01, btc.price - 1000, btc.price + 2000);
    std::cout << "Position Opened: " << (opened ? "Yes" : "No") << " Balance: $" << account.getBalance() << std::endl;
    bool closed = account.closePosition(0, 97000);
    std::cout << "Position Closed: " << (closed ? "Yes" : "No") << " Balance: $" << account.getBalance() << " Total PnL: $" << account.getTotalPnL() << std::endl;

    SignalManager manager;
    auto submitSignal = [&](SignalData signal) {
        std::string reason;
        if (RiskManager::validateSignal(signal, reason)) {
            manager.addSignal(signal);
            std::cout << "[APPROVED] " << signal.symbol << std::endl;
        }
        else {
            std::cout << "[REJECTED] " << signal.symbol << " - " << reason << std::endl;
        }
        };

    SignalData s1; s1.symbol = "BTC/USDT"; s1.type = SignalType::BUY; s1.entryPrice = btc.price; s1.stopLoss = btc.price - 1000; s1.takeProfit = btc.price + 2000; s1.confidence = 72.5;
    submitSignal(s1);
    SignalData s2; s2.symbol = "ETH/USDT"; s2.type = SignalType::BUY; s2.entryPrice = 3200; s2.stopLoss = 3150; s2.takeProfit = 3350; s2.confidence = 78.0;
    submitSignal(s2);
    SignalData s3; s3.symbol = "SOL/USDT"; s3.type = SignalType::SELL; s3.entryPrice = 180; s3.stopLoss = 185; s3.takeProfit = 170; s3.confidence = 69.5;
    submitSignal(s3);

    manager.displayAll();
    std::cout << "System Status: ONLINE" << std::endl;
    std::cout << "Signal Engine: READY" << std::endl;
    std::cout << "Total Managed Signals: " << manager.getCount() << std::endl;
    std::cout << "Trading Mode: PAPER TRADING" << std::endl;
    return 0;
}