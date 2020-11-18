#pragma once
#include <unordered_map>
#include "CEWin.h"

class CEWindowsMessage {
public:
	CEWindowsMessage() noexcept;
	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const noexcept;
private:
	std::unordered_map<DWORD, std::string> map;
};
