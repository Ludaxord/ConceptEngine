#pragma once
#include <chrono>

class CETimer {
public:
	CETimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point lastTimePoint;
};
