#pragma once
#include <unordered_map>
#include <Windows.h>

class WindowsMessage {
public:
	WindowsMessage() noexcept;
	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const noexcept;
private:
	std::unordered_map<DWORD, std::string> map;
};
