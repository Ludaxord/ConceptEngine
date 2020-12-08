#include "CETimer.h"

using namespace std::chrono;

CETimer::CETimer() {
	lastTimePoint = steady_clock::now();
}

float CETimer::Mark() {
	const auto oldTimePoint = lastTimePoint;
	lastTimePoint = steady_clock::now();
	const duration<float> frameTime = lastTimePoint - oldTimePoint;
	return frameTime.count();
}

float CETimer::Peek() const {
	return duration<float>(steady_clock::now() - lastTimePoint).count();
}
