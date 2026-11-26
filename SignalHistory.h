#pragma once
#include <string>
#include <vector>

namespace TradePilot {

    struct HistoryEntry {
        std::string symbol;
        std::string decision;
        double entryPrice;
        double confidence;
    };

    class SignalHistory {
    private:
        std::vector<HistoryEntry> entries;
        int maxEntries = 100;

    public:
        void addSignal(const std::string& symbol, const std::string& decision, double entryPrice, double confidence);
        int getCount() const;
        std::vector<HistoryEntry> getAll() const;
        std::vector<HistoryEntry> getLast(int n) const;
        void clearAll();
    };

}
