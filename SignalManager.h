#pragma once

#include <vector>
#include "SignalData.h"

class SignalManager
{
private:
    std::vector<SignalData> signals;

public:
    bool addSignal(const SignalData& signal);

    void displayAll() const;

    int getCount() const;
};