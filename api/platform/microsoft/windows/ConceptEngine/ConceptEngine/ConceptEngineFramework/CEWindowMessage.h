#pragma once

#include <unordered_map>
#include "CELib.h"

class CEWindowMessage {
public:
	CEWindowMessage() noexcept;
	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const noexcept;
private:
	std::unordered_map<DWORD, std::string> map;
};
