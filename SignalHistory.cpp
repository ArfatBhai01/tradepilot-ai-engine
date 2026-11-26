#include "SignalHistory.h"

namespace TradePilot {

    void SignalHistory::addSignal(const std::string& symbol, const std::string& decision, double entryPrice, double confidence) {
        HistoryEntry entry;
        entry.symbol = symbol;
        entry.decision = decision;
        entry.entryPrice = entryPrice;
        entry.confidence = confidence;
        entries.push_back(entry);

        while ((int)entries.size() > maxEntries) {
            entries.erase(entries.begin());
        }
    }

    int SignalHistory::getCount() const {
        return (int)entries.size();
    }

    std::vector<HistoryEntry> SignalHistory::getAll() const {
        return entries;
    }

    std::vector<HistoryEntry> SignalHistory::getLast(int n) const {
        std::vector<HistoryEntry> result;
        if (n <= 0 || entries.empty()) return result;
        int start = (int)entries.size() - n;
        if (start < 0) start = 0;
        for (int i = start; i < (int)entries.size(); i++) {
            result.push_back(entries[i]);
        }
        return result;
    }

    void SignalHistory::clearAll() {
        entries.clear();
    }

}