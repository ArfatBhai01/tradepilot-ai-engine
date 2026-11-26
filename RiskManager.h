#pragma once

#include "SignalData.h"
#include <string>

class RiskManager
{
public:
    static bool validateSignal(
        const SignalData& signal,
        std::string& reason
    );
};