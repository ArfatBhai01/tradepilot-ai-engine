#include "SignalManager.h"
#include <iostream>

bool SignalManager::addSignal(const SignalData& signal)
{
    if (!signal.isValid())
    {
        std::cout << "Invalid signal rejected!\n";
        return false;
    }

    signals.push_back(signal);
    return true;
}

void SignalManager::displayAll() const
{
    std::cout << "\n===== SIGNAL MANAGER =====\n";
    std::cout << "Total Signals: " << signals.size() << "\n";

    for (const SignalData& signal : signals)
    {
        signal.display();
    }
}

int SignalManager::getCount() const
{
    return static_cast<int>(signals.size());
}