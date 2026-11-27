#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
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

// Price ko sahi decimals mein dikhao — chhote coins (DOGE, ADA) ke liye zyada decimals,
// warna 2 decimals mein SL/TP entry jaisa hi dikhta hai.
std::string formatPrice(double p) {
    std::ostringstream os;
    os << std::fixed;
    if (p >= 1.0)       os << std::setprecision(2);
    else if (p >= 0.1)  os << std::setprecision(4);
    else if (p >= 0.01) os << std::setprecision(5);
    else                os << std::setprecision(6);
    os << p;
    return os.str();
}

int main() {
    std::cout << "==============================" << std::endl;
    std::cout << "      TRADEPILOT AI" << std::endl;
    std::cout << "  MULTI-SIGNAL SYSTEM" << std::endl;
    std::cout << "==============================" << std::endl;

    // Top 10 coins by market cap (CoinGecko, sab Binance USDT pairs par hain)
    std::vector<std::string> coins = {
        "BTCUSDT", "ETHUSDT", "BNBUSDT", "XRPUSDT", "SOLUSDT",
        "TRXUSDT", "HYPEUSDT", "ZECUSDT", "DOGEUSDT", "ADAUSDT"
    };

    // Env vars ek baar padho (loop se pehle)
    const char* _botToken = std::getenv("TELEGRAM_BOT_TOKEN");
    std::string botToken = _botToken ? _botToken : "";
    const char* _chatId = std::getenv("TELEGRAM_CHAT_ID");
    std::string chatId = _chatId ? _chatId : "";
    const char* _serverSecret = std::getenv("BOT_SERVER_SECRET");
    std::string serverSecret = _serverSecret ? _serverSecret : "";

    double btcPrice = 0;
    int sentCount = 0;

    for (const std::string& symbol : coins) {
        std::string display = symbol.substr(0, symbol.size() - 4) + "/USDT";

        double livePrice = 0;
        bool liveOk = TradePilot::fetchLivePrice(symbol, livePrice);
        if (!liveOk) {
            std::cout << symbol << ": Live price nahi mila, skip" << std::endl;
            continue;
        }
        if (symbol == "BTCUSDT") btcPrice = livePrice;
        std::cout << symbol << " Live Price: $" << livePrice << " (Binance)" << std::endl;

        MarketData md;
        md.symbol = display;
        md.price = livePrice;
        md.changePercent = 0.0;
        md.displayMarketInfo();

        std::vector<double> closes;
        bool candlesLive = TradePilot::fetchLiveCloses(symbol, 15, closes);
        if (!candlesLive) {
            std::cout << symbol << ": Live candles nahi mile, skip" << std::endl;
            continue;
        }
        std::cout << symbol << " Live Candles: " << closes.size() << " (Binance 1h)" << std::endl;

        std::vector<double> lows, highs;
        for (double c : closes) { lows.push_back(c * 0.995); highs.push_back(c * 1.005); }

        double sma = Indicators::calculateSMA(closes, 14);
        double ema = Indicators::calculateEMA(closes, 14);
        double rsi = Indicators::calculateRSI(closes, 14);
        double support = TradePilot::calculateSupport(lows, 14);
        double resistance = TradePilot::calculateResistance(highs, 14);

        std::cout << symbol << " SMA 14: " << sma << std::endl;
        std::cout << symbol << " EMA 14: " << ema << std::endl;
        std::cout << symbol << " RSI 14: " << rsi << std::endl;
        std::cout << symbol << " Support: " << support << std::endl;
        std::cout << symbol << " Resistance: " << resistance << std::endl;

        TradePilot::StrategyResult strat = TradePilot::evaluateStrategy(livePrice, ema, rsi, support, resistance);
        std::cout << symbol << " Strategy: " << TradePilot::decisionToString(strat.decision) << " Confidence " << strat.confidence << "%" << std::endl;
        std::cout << symbol << " Reason: " << strat.reason << std::endl;
        if (strat.decision == TradePilot::TradeDecision::NONE) {
            std::cout << symbol << ": No clear trend (NONE), signal nahi bheja jayega" << std::endl;
            continue;
        }
        std::string side = TradePilot::decisionToString(strat.decision);
        // SL/TP price ke % mein (har coin ke liye sahi): SL 1.5%, TP 3%
        double slDist = livePrice * 0.015;
        double tpDist = livePrice * 0.03;
        double sigSL = (side == "BUY") ? livePrice - slDist : livePrice + slDist;
        double sigTP = (side == "BUY") ? livePrice + tpDist : livePrice - tpDist;
        // Professional signal message (HTML bold + emojis, all English)
        std::string sideIcon = (side == "BUY") ? "\U0001F7E2" : "\U0001F534";
        std::string trendIcon = (side == "BUY") ? "\U0001F4C8" : "\U0001F4C9";
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "\U0001F680 <b>TradePilot AI | SIGNAL</b>\n\n";
        ss << sideIcon << " <b>" << side << "</b> " << trendIcon << " " << display << "\n";
        ss << "\U0001F4B0 Entry: <b>$" << formatPrice(livePrice) << "</b>\n";
        ss << "\U0001F6E1\U0000FE0F Stop Loss: $" << formatPrice(sigSL) << "\n";
        ss << "\U0001F3AF Take Profit: $" << formatPrice(sigTP) << "\n";
        ss << "\U0001F4CA RSI: " << rsi << "\n";
        ss << "\U0001F4AA Confidence: " << strat.confidence << "%\n";
        ss << "\n\U000026A0\U0000FE0F Paper trading only. No profit guarantee.";
        std::string signalText = ss.str();
        bool sent = TradePilot::sendTelegramSignal(botToken, chatId, signalText);
        (void)sent;
        // C++ signal ko server pe bhejo (Mini App mein dikhega)
        bool serverSent = TradePilot::sendSignalToServer("https://arfuu0.pythonanywhere.com", serverSecret, symbol, side, livePrice, sigSL, sigTP, strat.confidence, rsi);
        std::cout << symbol << " Server Signal Sent: " << (serverSent ? "Yes" : "No") << std::endl;
        sentCount++;
    }
    std::cout << "Total Signals Sent: " << sentCount << std::endl;

    TradePilot::SignalHistory history;
    history.addSignal("BTC/USDT", "BUY", btcPrice, 72.5);
    std::cout << "History Count: " << history.getCount() << std::endl;

    TradePilot::PaperTradingAccount account(10000);
    std::cout << "Paper Balance Start: $" << account.getBalance() << std::endl;
    bool opened = account.openPosition("BTC/USDT", "BUY", btcPrice, 0.01, btcPrice - 1000, btcPrice + 2000);
    std::cout << "Position Opened: " << (opened ? "Yes" : "No") << " Balance: " << account.getBalance() << std::endl;
    bool closed = account.closePosition(0, 97000);
    std::cout << "Position Closed: " << (closed ? "Yes" : "No") << " Balance: " << account.getBalance() << " Total PnL: " << account.getTotalPnL() << std::endl;

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

    SignalData s1; s1.symbol = "BTC/USDT"; s1.type = SignalType::BUY; s1.entryPrice = btcPrice; s1.stopLoss = btcPrice - 1000; s1.takeProfit = btcPrice + 2000; s1.confidence = 72.5;
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
