#pragma once
#include <vector>

namespace TradePilot {

	// Support: pichle 'period' candles mein sabse neeche wala low price
	// Example: period 20 matlab pichle 20 candles ka sabse low price
	double calculateSupport(const std::vector<double>& lows, int period);

	// Resistance: pichle 'period' candles mein sabse upar wala high price
	double calculateResistance(const std::vector<double>& highs, int period);

	// Kya price kisi level ke paas hai? tolerancePercent matlab kitne % paas hona chahiye
	// Example: price 100, level 101, tolerance 2% matlab paas hai
	bool isNearLevel(double price, double level, double tolerancePercent);

}
