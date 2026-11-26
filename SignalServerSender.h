#pragma once
#include <string>

namespace TradePilot {

    // C++ engine ka signal balance server ko bhejta hai.
    // serverUrl: "https://arfuu0.pythonanywhere.com"
    // secret: wahi BOT_SERVER_SECRET jo PythonAnywhere mein daala tha
    //         (code mein khud paste karo, chat mein mat bhejo)
    // side: "BUY" ya "SELL"
    bool sendSignalToServer(const std::string& serverUrl,
        const std::string& secret,
        const std::string& symbol,
        const std::string& side,
        double entry,
        double stopLoss,
        double takeProfit,
        double confidence,
        double rsi);

}
