#pragma once
#include "RefCountedObject.h"

#include "Containers/Utilities.h"

// CERef - Helper class when using objects with RefCountedObject as a base

template<typename T>
class CERef
{
public:
    template<typename TOther>
    friend class CERef;

    FORCEINLINE CERef() noexcept
        : RefPtr(nullptr)
    {
    }

    FORCEINLINE CERef(const CERef& Other) noexcept
        : RefPtr(Other.RefPtr)
    {
        AddRef();
    }

    template<typename TOther>
    FORCEINLINE CERef(const CERef<TOther>& Other) noexcept
        : RefPtr(Other.RefPtr)
    {
        static_assert(std::is_convertible<TOther*, T*>());
        AddRef();
    }

    FORCEINLINE CERef(CERef&& Other) noexcept
        : RefPtr(Other.RefPtr)
    {
        Other.RefPtr = nullptr;
    }

    template<typename TOther>
    FORCEINLINE CERef(CERef<TOther>&& Other) noexcept
        : RefPtr(Other.RefPtr)
    {
        static_assert(std::is_convertible<TOther*, T*>());
        Other.RefPtr = nullptr;
    }

    FORCEINLINE CERef(T* InPtr) noexcept
        : RefPtr(InPtr)
    {
    }

    template<typename TOther>
    FORCEINLINE CERef(TOther* InPtr) noexcept
        : RefPtr(InPtr)
    {
        static_assert(std::is_convertible<TOther*, T*>());
    }

    FORCEINLINE ~CERef()
    {
        Release();
    }

    FORCEINLINE T* Reset() noexcept
    {
        T* WeakPtr = RefPtr;
        Release();

        return WeakPtr;
    }

    FORCEINLINE T* ReleaseOwnership() noexcept
    {
        T* WeakPtr = RefPtr;
        RefPtr = nullptr;
        return WeakPtr;
    }

    FORCEINLINE void AddRef() noexcept
    {
        if (RefPtr)
        {
            RefPtr->AddRef();
        }
    }

    FORCEINLINE void Swap(T* InPtr) noexcept
    {
        Release();
        RefPtr = InPtr;
    }

    template<typename TOther>
    FORCEINLINE void Swap(TOther* InPtr) noexcept
    {
        static_assert(std::is_convertible<TOther*, T*>());

        Release();
        RefPtr = InPtr;
    }

    FORCEINLINE T* Get() const noexcept
    {
        return RefPtr;
    }

    FORCEINLINE T* GetRefCount() const noexcept
    {
        return RefPtr->GetRefCount();
    }

    FORCEINLINE T* GetAndAddRef() noexcept
    {
        AddRef();
        return RefPtr;
    }

    template<typename TCastType>
    FORCEINLINE TEnableIf<std::is_convertible_v<TCastType*, T*>, TCastType*> GetAs() const noexcept
    {
        return static_cast<TCastType*>(RefPtr);
    }

    FORCEINLINE T* const* GetAddressOf() const noexcept
    {
        return &RefPtr;
    }

    FORCEINLINE T& Dereference() const noexcept
    {
        return *RefPtr;
    }

    FORCEINLINE T* operator->() const noexcept
    {
        return Get();
    }

    FORCEINLINE T* const* operator&() const noexcept
    {
        return GetAddressOf();
    }

    FORCEINLINE T& operator*() const noexcept
    {
        return Dereference();
    }

    FORCEINLINE bool operator==(T* InPtr) const noexcept
    {
        return (RefPtr == InPtr);
    }

    FORCEINLINE bool operator==(const CERef& Other) const noexcept
    {
        return (RefPtr == Other.RefPtr);
    }

    FORCEINLINE bool operator!=(T* InPtr) const noexcept
    {
        return (RefPtr != InPtr);
    }

    FORCEINLINE bool operator!=(const CERef& Other) const noexcept
    {
        return (RefPtr != Other.RefPtr);
    }

    FORCEINLINE bool operator==(std::nullptr_t) const noexcept
    {
        return (RefPtr == nullptr);
    }

    FORCEINLINE bool operator!=(std::nullptr_t) const noexcept
    {
        return (RefPtr != nullptr);
    }

    template<typename TOther>
    FORCEINLINE TEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator==(TOther* RHS) const noexcept
    {
        return (RefPtr == RHS);
    }

    template<typename TOther>
    friend FORCEINLINE TEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator==(TOther* LHS, const CERef& RHS) noexcept
    {
        return (RHS == LHS);
    }

    template<typename TOther>
    FORCEINLINE TEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator==(const CERef<TOther>& RHS) const noexcept
    {
        return (RefPtr == RHS.RefPtr);
    }

    template<typename TOther>
    FORCEINLINE TEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator!=(TOther* RHS) const noexcept
    {
        return (RefPtr != RHS);
    }

    template<typename TOther>
    friend FORCEINLINE TEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator!=(TOther* LHS, const CERef& RHS) noexcept
    {
        return (RHS != LHS);
    }

    template<typename TOther>
    FORCEINLINE TEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator!=(const CERef<TOther>& RHS) const noexcept
    {
        return (RefPtr != RHS.RefPtr);
    }

    FORCEINLINE operator bool() const noexcept
    {
        return (RefPtr != nullptr);
    }

    FORCEINLINE CERef& operator=(const CERef& Other) noexcept
    {
        if (this != std::addressof(Other))
        {
            Release();

            RefPtr = Other.RefPtr;
            AddRef();
        }

        return *this;
    }

    template<typename TOther>
    FORCEINLINE CERef& operator=(const CERef<TOther>& Other) noexcept
    {
        static_assert(std::is_convertible<TOther*, T*>());

        if (this != std::addressof(Other))
        {
            Release();

            RefPtr = Other.RefPtr;
            AddRef();
        }

        return *this;
    }

    FORCEINLINE CERef& operator=(CERef&& Other) noexcept
    {
        if (this != std::addressof(Other))
        {
            Release();

            RefPtr			= Other.RefPtr;
            Other.RefPtr	= nullptr;
        }

        return *this;
    }

    template<typename TOther>
    FORCEINLINE CERef& operator=(CERef<TOther>&& Other) noexcept
    {
        static_assert(std::is_convertible<TOther*, T*>());

        if (this != std::addressof(Other))
        {
            Release();

            RefPtr			= Other.RefPtr;
            Other.RefPtr	= nullptr;
        }

        return *this;
    }

    FORCEINLINE CERef& operator=(T* InPtr) noexcept
    {
        if (RefPtr != InPtr)
        {
            Release();
            RefPtr = InPtr;
        }

        return *this;
    }

    template<typename TOther>
    FORCEINLINE CERef& operator=(TOther* InPtr) noexcept
    {
        static_assert(std::is_convertible<TOther*, T*>());

        if (RefPtr != InPtr)
        {
            Release();
            RefPtr = InPtr;
        }

        return *this;
    }

    FORCEINLINE CERef& operator=(std::nullptr_t) noexcept
    {
        Release();
        return *this;
    }

private:
    FORCEINLINE void Release() noexcept
    {
        if (RefPtr)
        {
            RefPtr->Release();
            RefPtr = nullptr;
        }
    }

    T* RefPtr;
};

// static_cast
template<typename T, typename U>
CERef<T> StaticCast(const CERef<U>& Pointer)
{
    T* RawPointer = static_cast<T*>(Pointer.Get());
    RawPointer->AddRef();
    return CERef<T>(RawPointer);
}

template<typename T, typename U>
CERef<T> StaticCast(CERef<U>&& Pointer)
{
    T* RawPointer = static_cast<T*>(Pointer.Get());
    return CERef<T>(RawPointer);
}

// const_cast
template<typename T, typename U>
CERef<T> ConstCast(const CERef<U>& Pointer)
{
    T* RawPointer = const_cast<T*>(Pointer.Get());
    RawPointer->AddRef();
    return CERef<T>(RawPointer);
}

template<typename T, typename U>
CERef<T> ConstCast(CERef<U>&& Pointer)
{
    T* RawPointer = const_cast<T*>(Pointer.Get());
    return CERef<T>(RawPointer);
}

// reinterpret_cast
template<typename T, typename U>
CERef<T> ReinterpretCast(const CERef<U>& Pointer)
{
    T* RawPointer = reinterpret_cast<T*>(Pointer.Get());
    RawPointer->AddRef();
    return CERef<T>(RawPointer);
}

template<typename T, typename U>
CERef<T> ReinterpretCast(CERef<U>&& Pointer)
{
    T* RawPointer = reinterpret_cast<T*>(Pointer.Get());
    return CERef<T>(RawPointer);
}

// dynamic_cast
template<typename T, typename U>
CERef<T> DynamicCast(const CERef<U>& Pointer)
{
    T* RawPointer = dynamic_cast<T*>(Pointer.Get());
    RawPointer->AddRef();
    return CERef<T>(RawPointer);
}

template<typename T, typename U>
CERef<T> DynamicCast(CERef<U>&& Pointer)
{
    T* RawPointer = dynamic_cast<T*>(Pointer.Get());
    return CERef<T>(RawPointer);
}

// Converts a raw pointer into a CERef
template<typename T, typename U>
CERef<T> MakeSharedRef(U* InRefCountedObject)
{
    if (InRefCountedObject)
    {
        InRefCountedObject->AddRef();
        return CERef<T>(static_cast<T*>(InRefCountedObject));
    }

    return CERef<T>();
}