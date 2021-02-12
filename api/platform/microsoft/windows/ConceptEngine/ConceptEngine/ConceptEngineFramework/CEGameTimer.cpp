#include "CEGameTimer.h"
#include "CEWinLib.h"

using namespace ConceptFramework::GameEngine;

using namespace std::chrono;

#define WINDOWS_CLOCK 0
#define HIGH_RESOLUTION_CLOCK 1
#define STEADY_CLOCK 2

#ifndef USE_CLOCK
#define USE_CLOCK HIGH_RESOLUTION_CLOCK
#endif

#if USE_CLOCK == WINDOWS_CLOCK
class CEGameTimer::impl {
public:
	impl() : t0{ 0 }, t1{ 0 }, frequency{ 0 }, elapsedNanoseconds(0.0), totalNanoseconds(0.0) {
		::QueryPerformanceFrequency(&frequency);
		::QueryPerformanceCounter(&t0)
	}

	void Tick() {
		::QueryPerformanceCounter(&t1);
		elapsedNanoseconds = (t1.QuadPart - t0.QuadPart) * (1e9 / frequency.QuadPart);
		totalNanoseconds += elapsedNanoseconds;
	}

	void Reset() {
		::QueryPerformanceCounter(&t0);
		elapsedNanoseconds = 0.0;
		totalNanoseconds = 0.0;
	}

	double ElapsedNanoseconds() const {
		return elapsedNanoseconds;
	}

	double TotalNanoseconds( ) const {
		return totalNanoseconds;
	}
protected:
private:
	LARGE_INTEGER t0, t1;
	LARGE_INTEGER frequency;
	double elapsedNanoseconds;
	double totalNanoseconds;
};
#elif USE_CLOCK == HIGH_RESOLUTION_CLOCK
class CEGameTimer::impl {
public:
	impl() : elapsedTime(0.0), totalTime(0.0) {
		t0 = high_resolution_clock::now();
	}

	void Tick() {
		t1 = high_resolution_clock::now();
		duration<double, std::nano> delta = t1 - t0;
		t0 = t1;
		elapsedTime = delta.count();
		totalTime += elapsedTime;
	}

	void Reset() {
		t0 = high_resolution_clock::now();
		elapsedTime = 0.0;
		totalTime = 0.0;
	}

	double ElapsedNanoseconds() const {
		return elapsedTime;
	}

	double TotalNanoseconds() const {
		return totalTime;
	}

protected:
private:
	high_resolution_clock::time_point t0, t1;
	double elapsedTime;
	double totalTime;
};
#elif USE_CLOCK == STEADY_CLOCK
class CEGameTimer::impl {
public:
	impl() : elapsedTime(0.0), totalTime(0.0) {
		t0 = steady_clock::now();
	}

	void Tick() {
		t1 = steady_clock::now();
		duration<double, std::nano> delta = t1 - t0;
		t0 = t1;
		elapsedTime = delta.count();
		totalTime += elapsedTime;
	}

	void Reset() {
		t0 = steady_clock::now();
		elapsedTime = 0.0;
		totalTime = 0.0;
	}

	double ElapsedNanoseconds() const {
		return elapsedTime;
	}

	double TotalNanoseconds() const {
		return totalTime;
	}

protected:
private:
	steady_clock::time_point t0, t1;
	double elapsedTime;
	double totalTime;
};
#endif

CEGameTimer::CEGameTimer() {
	pImpl = std::make_unique<impl>();
}

CEGameTimer::~CEGameTimer() {
}

void CEGameTimer::Tick() {
	pImpl->Tick();
}

void CEGameTimer::Reset() {
	pImpl->Reset();
}

double CEGameTimer::ElapsedSeconds() const {
	return pImpl->ElapsedNanoseconds() * 1e-9;
}

double CEGameTimer::ElapsedMilliseconds() const {
	return pImpl->ElapsedNanoseconds() * 1e-6;
}

double CEGameTimer::ElapsedMicroseconds() const {
	return pImpl->ElapsedNanoseconds() * 1e-3;
}

double CEGameTimer::ElapsedNanoseconds() const {
	return pImpl->ElapsedNanoseconds();
}

double CEGameTimer::TotalSeconds() const {
	return pImpl->TotalNanoseconds() * 1e-9;
}

double CEGameTimer::TotalMilliseconds() const {
	return pImpl->TotalNanoseconds() * 1e-6;
}

double CEGameTimer::TotalMicroseconds() const {
	return pImpl->TotalNanoseconds() * 1e-3;
}

double CEGameTimer::TotalNanoseconds() const {
	return pImpl->TotalNanoseconds();
}
