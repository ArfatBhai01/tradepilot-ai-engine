
#include "Indicators.h"

#include <cmath>
#include <limits>
#include <numeric>

namespace
{
    const double INVALID_VALUE =
        std::numeric_limits<double>::quiet_NaN();

    std::vector<double> calculateEMASeries(
        const std::vector<double>& prices,
        std::size_t period)
    {
        std::vector<double> result(
            prices.size(), INVALID_VALUE
        );

        if (period == 0 || prices.size() < period)
            return result;

        for (double price : prices)
        {
            if (!std::isfinite(price) || price <= 0)
                return result;
        }

        double sum = 0.0;

        for (std::size_t i = 0; i < period; ++i)
        {
            sum += prices[i];
        }

        double ema = sum / static_cast<double>(period);

        result[period - 1] = ema;

        double multiplier =
            2.0 / (static_cast<double>(period) + 1.0);

        for (std::size_t i = period; i < prices.size(); ++i)
        {
            ema = (prices[i] - ema) * multiplier + ema;
            result[i] = ema;
        }

        return result;
    }
}

double Indicators::calculateSMA(
    const std::vector<double>& prices,
    std::size_t period)
{
    if (period == 0 || prices.size() < period)
        return INVALID_VALUE;

    double sum = 0.0;

    for (std::size_t i = prices.size() - period;
        i < prices.size(); ++i)
    {
        if (!std::isfinite(prices[i]) || prices[i] <= 0)
            return INVALID_VALUE;

        sum += prices[i];
    }

    return sum / static_cast<double>(period);
}

double Indicators::calculateEMA(
    const std::vector<double>& prices,
    std::size_t period)
{
    std::vector<double> values =
        calculateEMASeries(prices, period);

    if (values.empty() || !std::isfinite(values.back()))
        return INVALID_VALUE;

    return values.back();
}

double Indicators::calculateRSI(
    const std::vector<double>& prices,
    std::size_t period)
{
    if (period == 0 || prices.size() <= period)
        return INVALID_VALUE;

    for (double price : prices)
    {
        if (!std::isfinite(price) || price <= 0)
            return INVALID_VALUE;
    }

    double averageGain = 0.0;
    double averageLoss = 0.0;

    for (std::size_t i = 1; i <= period; ++i)
    {
        double change = prices[i] - prices[i - 1];

        if (change > 0)
            averageGain += change;
        else
            averageLoss -= change;
    }

    averageGain /= static_cast<double>(period);
    averageLoss /= static_cast<double>(period);

    for (std::size_t i = period + 1;
        i < prices.size(); ++i)
    {
        double change = prices[i] - prices[i - 1];

        double gain = change > 0 ? change : 0.0;
        double loss = change < 0 ? -change : 0.0;

        averageGain =
            (averageGain * (period - 1) + gain) / period;

        averageLoss =
            (averageLoss * (period - 1) + loss) / period;
    }

    if (averageGain == 0.0 && averageLoss == 0.0)
        return 50.0;

    if (averageLoss == 0.0)
        return 100.0;

    double relativeStrength = averageGain / averageLoss;

    return 100.0 - (100.0 / (1.0 + relativeStrength));
}

MACDResult Indicators::calculateMACD(
    const std::vector<double>& prices,
    std::size_t fastPeriod,
    std::size_t slowPeriod,
    std::size_t signalPeriod)
{
    MACDResult result{ 0.0, 0.0, 0.0, false };

    if (fastPeriod == 0 ||
        slowPeriod == 0 ||
        signalPeriod == 0 ||
        fastPeriod >= slowPeriod ||
        prices.size() < slowPeriod)
    {
        return result;
    }

    std::vector<double> fastEMA =
        calculateEMASeries(prices, fastPeriod);

    std::vector<double> slowEMA =
        calculateEMASeries(prices, slowPeriod);

    std::vector<double> macdValues;

    for (std::size_t i = slowPeriod - 1;
        i < prices.size(); ++i)
    {
        if (!std::isfinite(fastEMA[i]) ||
            !std::isfinite(slowEMA[i]))
        {
            return result;
        }

        macdValues.push_back(fastEMA[i] - slowEMA[i]);
    }

    if (macdValues.size() < signalPeriod)
        return result;

    std::vector<double> signalEMA =
        calculateEMASeries(macdValues, signalPeriod);

    if (signalEMA.empty() ||
        !std::isfinite(signalEMA.back()))
    {
        return result;
    }

    result.macdLine = macdValues.back();
    result.signalLine = signalEMA.back();

    result.histogram =
        result.macdLine - result.signalLine;

    result.valid = true;

    return result;
}
