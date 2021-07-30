#pragma once
#include "../CEDefinitions.h"

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(push)
    #pragma warning(disable : 4251)
#endif

class CETimestamp
{
public:
    FORCEINLINE CETimestamp(uint64 Nanoseconds = 0)
        : TimestampInNS(Nanoseconds)
    {
    }

    FORCEINLINE double AsSeconds() const
    {
        constexpr double SECONDS = 1000.0 * 1000.0 * 1000.0;
        return double(TimestampInNS) / SECONDS;
    }

    FORCEINLINE double AsMilliSeconds() const
    {
        constexpr double MILLISECONDS = 1000.0 * 1000.0;
        return double(TimestampInNS) / MILLISECONDS;
    }

    FORCEINLINE double AsMicroSeconds() const
    {
        constexpr double MICROSECONDS = 1000.0;
        return double(TimestampInNS) / MICROSECONDS;
    }

    FORCEINLINE uint64 AsNanoSeconds() const
    {
        return TimestampInNS;
    }

    FORCEINLINE bool operator==(const CETimestamp& Other) const
    {
        return TimestampInNS == Other.TimestampInNS;
    }

    FORCEINLINE bool operator!=(const CETimestamp& Other) const
    {
        return TimestampInNS != Other.TimestampInNS;
    }

    FORCEINLINE CETimestamp& operator+=(const CETimestamp& Right)
    {
        TimestampInNS += Right.TimestampInNS;
        return *this;
    }

    FORCEINLINE CETimestamp& operator-=(const CETimestamp& Right)
    {
        TimestampInNS -= Right.TimestampInNS;
        return *this;
    }

    FORCEINLINE CETimestamp& operator*=(const CETimestamp& Right)
    {
        TimestampInNS *= Right.TimestampInNS;
        return *this;
    }

    FORCEINLINE CETimestamp& operator/=(const CETimestamp& Right)
    {
        TimestampInNS /= Right.TimestampInNS;
        return *this;
    }

    FORCEINLINE static CETimestamp Seconds(double Seconds)
    {
        constexpr double SECOND = 1000.0 * 1000.0 * 1000.0;
        return CETimestamp(static_cast<uint64>(Seconds * SECOND));
    }

    FORCEINLINE static CETimestamp MilliSeconds(double Milliseconds)
    {
        constexpr double MILLISECOND = 1000.0 * 1000.0;
        return CETimestamp(static_cast<uint64>(Milliseconds * MILLISECOND));
    }

    FORCEINLINE static CETimestamp MicroSeconds(double Microseconds)
    {
        constexpr double MICROSECOND = 1000.0;
        return CETimestamp(static_cast<uint64>(Microseconds * MICROSECOND));
    }

    FORCEINLINE static CETimestamp NanoSeconds(uint64 Nanoseconds)
    {
        return CETimestamp(Nanoseconds);
    }

public:
    FORCEINLINE friend CETimestamp operator+(const CETimestamp& Left, const CETimestamp& Right)
    {
        return CETimestamp(Left.TimestampInNS + Right.TimestampInNS);
    }

    FORCEINLINE friend CETimestamp operator-(const CETimestamp& Left, const CETimestamp& Right)
    {
        return CETimestamp(Left.TimestampInNS - Right.TimestampInNS);
    }

    FORCEINLINE friend CETimestamp operator*(const CETimestamp& Left, const CETimestamp& Right)
    {
        return CETimestamp(Left.TimestampInNS * Right.TimestampInNS);
    }

    FORCEINLINE friend CETimestamp operator/(const CETimestamp& Left, const CETimestamp& Right)
    {
        return CETimestamp(Left.TimestampInNS / Right.TimestampInNS);
    }

    FORCEINLINE friend bool operator>(const CETimestamp& Left, const CETimestamp& Right)
    {
        return Left.TimestampInNS > Right.TimestampInNS;
    }

    FORCEINLINE friend bool operator<(const CETimestamp& Left, const CETimestamp& Right)
    {
        return Left.TimestampInNS < Right.TimestampInNS;
    }

    FORCEINLINE friend bool operator>=(const CETimestamp& Left, const CETimestamp& Right)
    {
        return (Left.TimestampInNS >= Right.TimestampInNS);
    }

    FORCEINLINE friend bool operator<=(const CETimestamp& Left, const CETimestamp& Right)
    {
        return (Left.TimestampInNS <= Right.TimestampInNS);
    }

private:
    uint64 TimestampInNS = 0;
};

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(pop)
#endif