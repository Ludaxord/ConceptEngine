#include <windows.h>
#include "CETimer.h"
#include "../Tools/CEUtils.h"

CETimer::CETimer()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
	  mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false) {
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;

	CE_LOG(" Seconds Per Count: " + std::to_string(mSecondsPerCount) + " countsPerSec: "+ std::to_string(countsPerSec));
}

// Returns the total time elapsed since Reset() was called, NOT counting any
// time when the clock is stopped.
float CETimer::TotalTime() const {
	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance 
	// mStopTime - mBaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from mStopTime:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

	if (mStopped) {
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}

		// The distance mCurrTime - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from mCurrTime:  
		//
		//  (mCurrTime - mPausedTime) - mBaseTime 
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mCurrTime

	else {
		return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}

float CETimer::DeltaTime() const {
	return (float)mDeltaTime;
}

void CETimer::Reset() {
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void CETimer::Start() {
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	CE_LOG("Start Time: " + std::to_string(startTime));

	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     

	if (mStopped) {
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void CETimer::Stop() {
	if (!mStopped) {
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

void CETimer::Update() {
	if (mStopped) {
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	// Time difference between this frame and the previous.
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	// Prepare for next frame.
	mPrevTime = mCurrTime;

	// CE_LOG(
	// 	"Current Time: " + std::to_string(mCurrTime) + " Previous Time: " + std::to_string(mPrevTime) +
	// 	" Seconds Per Count: " + std::to_string(mSecondsPerCount) + " Delta Time: "+ std::to_string(mDeltaTime));
	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.

	if (mDeltaTime < 0.0) {
		mDeltaTime = 0.0;
	}
}
