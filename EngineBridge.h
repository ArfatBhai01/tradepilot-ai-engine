#pragma once
#include <string>

namespace TradePilot {
	// fullUrl = backend page wala pura heartbeat link
	// apiKey = tumhari tpk_ wali key
	// totalSignals = manager.getCount()
	bool sendHeartbeat(const std::string& fullUrl, const std::string& apiKey, int totalSignals);
}
