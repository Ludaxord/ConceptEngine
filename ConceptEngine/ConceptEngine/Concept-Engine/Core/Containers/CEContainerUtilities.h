#pragma once

#include "../Common/CETypes.h"

/*
 * TRemoveReference - Removes reference and retrives the types
 */

template<typename T>
struct _CERemoveReference
{
    using TType = T;
};

template<typename T>
struct _CERemoveReference<T&>
{
    using TType = T;
};

template<typename T>
struct _CERemoveReference<T&&>
{
    using TType = T;
};

template<typename T>
using CERemoveReference = typename _CERemoveReference<T>::TType;

/*
 * TRemovePointer - Removes pointer and retrives the type
 */

template<typename T>
struct _CERemovePointer
{
    using TType = T;
};

template<typename T>
struct _CERemovePointer<T*>
{
    using TType = T;
};

template<typename T>
struct _CERemovePointer<T* const>
{
    using TType = T;
};

template<typename T>
struct _CERemovePointer<T* volatile>
{
    using TType = T;
};

template<typename T>
struct _CERemovePointer<T* const volatile>
{
    using TType = T;
};

template<typename T>
using CERemovePointer = typename _CERemovePointer<T>::TType;

/*
 * TRemoveExtent - Removes array type
 */

template<typename T>
struct _CERemoveExtent
{
    using TType = T;
};

template<typename T>
struct _CERemoveExtent<T[]>
{
    using TType = T;
};

template<typename T, size_t SIZE>
struct _CERemoveExtent<T[SIZE]>
{
    using TType = T;
};

template<typename T>
using CERemoveExtent = typename _CERemoveExtent<T>::TType;

/*
 * Move
 */

// Move an object by converting it into a rvalue
template<typename T>
constexpr CERemoveReference<T>&& Move(T&& Arg) noexcept
{
    return static_cast<CERemoveReference<T>&&>(Arg);
}

/*
 * Forward
 */

// Forward an object by converting it into a rvalue from an lvalue
template<typename T>
constexpr T&& Forward(CERemoveReference<T>& Arg) noexcept
{
    return static_cast<T&&>(Arg);
}

// Forward an object by converting it into a rvalue from an rvalue
template<typename T>
constexpr T&& Forward(CERemoveReference<T>&& Arg) noexcept
{
    return static_cast<T&&>(Arg);
}

/*
 * TEnableIf
 */

template<bool B, typename T = void>
struct _CEEnableIf
{
};

template<typename T>
struct _CEEnableIf<true, T>
{
    using TType = T;
};

template<bool B, typename T = void>
using CEEnableIf = typename _CEEnableIf<B, T>::TType;

/*
 * TIsArray - Check for array type
 */

template<typename T>
struct _CEIsArray
{
    static constexpr bool Value = false;
};

template<typename T>
struct _CEIsArray<T[]>
{
    static constexpr bool Value = true;
};

template<typename T, int32 N>
struct _CEIsArray<T[N]>
{
    static constexpr bool Value = true;
};

template<typename T>
inline constexpr bool CEIsArray = _CEIsArray<T>::Value;