#pragma once
#include "Utilities.h"
#include "Iterator.h"

// CEArrayView - View of an array similar to std::span

template<typename T>
class CEArrayView
{
public:
    typedef T*                        Iterator;
    typedef const T*                  ConstIterator;
    typedef TReverseIterator<T>       ReverseIterator;
    typedef TReverseIterator<const T> ConstReverseIterator;
    typedef uint32                    SizeType;

    CEArrayView() noexcept
        : View(nullptr)
        , ViewSize(0)
    {
    }

    template<typename CEArrayType>
    explicit CEArrayView(CEArrayType& Array) noexcept
        : View(Array.Data())
        , ViewSize(Array.Size())
    {
    }
    
    template<const SizeType N>
    explicit CEArrayView(T(&Array)[N]) noexcept
        : View(Array)
        , ViewSize(N)
    {
    }

    template<typename TInputIterator>
    explicit CEArrayView(TInputIterator Begin, TInputIterator End) noexcept
        : View(Begin)
        , ViewSize(SizeType(End - Begin))
    {
    }

    CEArrayView(const CEArrayView& Other) noexcept
        : View(Other.View)
        , ViewSize(Other.ViewSize)
    {
    }

    CEArrayView(CEArrayView&& Other) noexcept
        : View(Other.View)
        , ViewSize(Other.ViewSize)
    {
        Other.View = nullptr;
        Other.ViewSize = 0;
    }

    bool IsEmpty() const noexcept { return (ViewSize == 0); }

    T& Front() noexcept { return View[0]; }
    const T& Front() const noexcept { return View[0]; }

    T& Back() noexcept { return View[ViewSize - 1]; }
    const T& Back() const noexcept { return View[ViewSize - 1]; }

    T& At(SizeType Index) noexcept
    {
        Assert(Index < ViewSize);
        return View[Index];
    }

    const T& At(SizeType Index) const noexcept
    {
        Assert(Index < ViewSize);
        return View[Index];
    }

    void Swap(CEArrayView& Other) noexcept
    {
        CEArrayView TempView(::Move(*this));
        *this = ::Move(Other);
        Other = ::Move(TempView);
    }
    
    Iterator Begin() noexcept { return Iterator(View); }
    Iterator End() noexcept { return Iterator(View + ViewSize); }

    ConstIterator Begin() const noexcept { return Iterator(View); }
    ConstIterator End() const noexcept { return Iterator(View + ViewSize); }

    SizeType LastIndex() const noexcept { return ViewSize > 0 ? ViewSize - 1 : 0; }
    SizeType Size() const noexcept { return ViewSize; }
    SizeType SizeInBytes() const noexcept { return ViewSize * sizeof(T); }

    T* Data() noexcept { return View; }
    const T* Data() const noexcept { return View; }

    T& operator[](SizeType Index) noexcept { return At(Index); }
    const T& operator[](SizeType Index) const noexcept { return At(Index); }

    CEArrayView& operator=(const CEArrayView& Other) noexcept
    {
        View = Other.View;
        ViewSize = Other.ViewSize;
        return *this;
    }

    CEArrayView& operator=(CEArrayView&& Other) noexcept
    {
        if (this != &Other)
        {
            View = Other.View;
            ViewSize = Other.ViewSize;
            Other.View = nullptr;
            Other.ViewSize = 0;
        }

        return *this;
    }

    // STL iterator functions - Enables Range-based for-loops
public:
    Iterator begin() noexcept { return View; }
    Iterator end() noexcept { return View + ViewSize; }

    ConstIterator begin() const noexcept { return View; }
    ConstIterator end() const noexcept { return View + ViewSize; }

    ConstIterator cbegin() const noexcept { return View; }
    ConstIterator cend() const noexcept { return View + ViewSize; }

    ReverseIterator rbegin() noexcept { return ReverseIterator(end()); }
    ReverseIterator rend() noexcept { return ReverseIterator(begin()); }

    ConstReverseIterator rbegin() const noexcept { return ConstReverseIterator(end()); }
    ConstReverseIterator rend() const noexcept { return ConstReverseIterator(begin()); }

    ConstReverseIterator crbegin() const noexcept { return ConstReverseIterator(end()); }
    ConstReverseIterator crend() const noexcept { return ConstReverseIterator(begin()); }

private:
    T*       View;
    SizeType ViewSize;
};
