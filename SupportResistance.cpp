#include "SupportResistance.h"
#include <cmath>

namespace TradePilot {

    double calculateSupport(const std::vector<double>& lows, int period) {
        if (lows.size() < (size_t)period || period <= 0) {
            return 0.0;
        }
        double lowest = lows[lows.size() - period];
        for (size_t i = lows.size() - period; i < lows.size(); i++) {
            if (lows[i] < lowest) lowest = lows[i];
        }
        return lowest;
    }

    double calculateResistance(const std::vector<double>& highs, int period) {
        if (highs.size() < (size_t)period || period <= 0) {
            return 0.0;
        }
        double highest = highs[highs.size() - period];
        for (size_t i = highs.size() - period; i < highs.size(); i++) {
            if (highs[i] > highest) highest = highs[i];
        }
        return highest;
    }

    bool isNearLevel(double price, double level, double tolerancePercent) {
        if (level == 0.0) return false;
        double diff = std::abs(price - level) / level * 100.0;
        return diff <= tolerancePercent;
    }

}