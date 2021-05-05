#pragma once
#include <windows.h>
#include "../Core/Common/CETypes.h"

#ifdef COMPILER_VISUAL_STUDIO
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace ConceptEngine::Time {
	class CETimestamp {
	public:
		FORCEINLINE CETimestamp(uint64 Nanoseconds = 0) : TimestampInNS(Nanoseconds) {

		}

		FORCEINLINE double AsSeconds() const {
			constexpr double SECONDS = 1000.0 * 1000.0 * 1000.0;
			return static_cast<double>(TimestampInNS) / SECONDS;
		}

		FORCEINLINE double AsMilliSeconds() const {
			constexpr double MILLISECONDS = 1000.0 * 1000.0;
			return static_cast<double>(TimestampInNS) / MILLISECONDS;
		}

		FORCEINLINE double AsMicroSeconds() const {
			constexpr double MICROSECONDS = 1000.0;
			return double(TimestampInNS) / MICROSECONDS;
		}

		FORCEINLINE uint64 AsNanoSeconds() const {
			return TimestampInNS;
		}

		FORCEINLINE bool operator==(const CETimestamp& Another) const {
			return TimestampInNS == Another.TimestampInNS;
		}

		FORCEINLINE bool operator!=(const CETimestamp& Another) const {
			return TimestampInNS != Another.TimestampInNS;
		}

		FORCEINLINE CETimestamp& operator+=(const CETimestamp& Another) {
			TimestampInNS += Another.TimestampInNS;
			return *this;
		}

		FORCEINLINE CETimestamp& operator-=(const CETimestamp& Another) {
			TimestampInNS -= Another.TimestampInNS;
			return *this;
		}

		FORCEINLINE CETimestamp& operator*=(const CETimestamp& Another) {
			TimestampInNS *= Another.TimestampInNS;
			return *this;
		}

		FORCEINLINE CETimestamp& operator/=(const CETimestamp& Another) {
			TimestampInNS /= Another.TimestampInNS;
			return *this;
		}

		FORCEINLINE static CETimestamp Seconds(double Seconds) {
			constexpr double SECONDS = 1000.0 * 1000.0 * 1000.0;
			return CETimestamp(static_cast<uint64>(Seconds * SECONDS));
		}

		FORCEINLINE static CETimestamp MilliSeconds(double MilliSeconds) {
			constexpr double MILLISECONDS = 1000.0 * 1000.0;
			return CETimestamp(static_cast<uint64>(MilliSeconds * MILLISECONDS));
		}

		FORCEINLINE static CETimestamp MicroSeconds(double MicroSeconds) {
			constexpr double MICROSECONDS = 1000.0;
			return CETimestamp(static_cast<uint64>(MicroSeconds * MICROSECONDS));
		}

		FORCEINLINE static CETimestamp NanoSeconds(double NanoSeconds) {
			return CETimestamp(static_cast<uint64>(NanoSeconds));
		}

		FORCEINLINE friend CETimestamp operator+(const CETimestamp& First, const CETimestamp& Second) {
			return CETimestamp(First.TimestampInNS + Second.TimestampInNS);
		}

		FORCEINLINE friend CETimestamp operator-(const CETimestamp& First, const CETimestamp& Second) {
			return CETimestamp(First.TimestampInNS - Second.TimestampInNS);
		}

		FORCEINLINE friend CETimestamp operator*(const CETimestamp& First, const CETimestamp& Second) {
			return CETimestamp(First.TimestampInNS * Second.TimestampInNS);
		}

		FORCEINLINE friend CETimestamp operator/(const CETimestamp& First, const CETimestamp& Second) {
			return CETimestamp(First.TimestampInNS / Second.TimestampInNS);
		}

		FORCEINLINE friend bool operator>(const CETimestamp& First, const CETimestamp& Second) {
			return First.TimestampInNS > Second.TimestampInNS;
		}

		FORCEINLINE friend bool operator<(const CETimestamp& First, const CETimestamp& Second) {
			return First.TimestampInNS < Second.TimestampInNS;
		}

		FORCEINLINE friend bool operator>=(const CETimestamp& First, const CETimestamp& Second) {
			return First.TimestampInNS >= Second.TimestampInNS;
		}

		FORCEINLINE friend bool operator<=(const CETimestamp& First, const CETimestamp& Second) {
			return First.TimestampInNS <= Second.TimestampInNS;
		}

	private:
		uint64 TimestampInNS = 0;
	};
}

#ifdef COMPILER_VISUAL_STUDIO
#pragma warning(pop)
#endif
