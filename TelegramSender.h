#pragma once
#include <string>

namespace TradePilot {
	// botToken = BotFather wala token, chatId = userinfobot wala number, text = signal message
	bool sendTelegramSignal(const std::string& botToken, const std::string& chatId, const std::string& text);
}
