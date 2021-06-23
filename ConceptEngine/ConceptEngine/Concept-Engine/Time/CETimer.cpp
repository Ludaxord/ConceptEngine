#include "CETimer.h"

#include "../Core/Platform/Generic/Time/CEPlatformTime.h"

ConceptEngine::Time::CETimer::CETimer() {
	Frequency = CEPlatformTime::QueryPerformanceFrequency();
	Update();
}

void ConceptEngine::Time::CETimer::Update() {
	const uint64 now = CEPlatformTime::QueryPerformanceCounter();
	constexpr uint64 NANOSECONDS = 1000 * 1000 * 1000;
	uint64 delta = now - LastTime;
	uint64 nanoseconds = (delta * NANOSECONDS) / Frequency;

	DeltaTime = CETimestamp(nanoseconds);
	LastTime = now;
	TotalTime += DeltaTime;
}
