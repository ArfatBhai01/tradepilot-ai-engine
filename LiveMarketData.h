#pragma once
#include <string>

namespace TradePilot {

	// Binance se live price lata hai, koi API key nahi chahiye
	// symbol aise likhna hai: "BTCUSDT", "ETHUSDT", "SOLUSDT"
	// Kaam ho jaye to true dega aur outPrice mein price aa jayega
	// Internet na ho to false dega
	bool fetchLivePrice(const std::string& symbol, double& outPrice);

}
