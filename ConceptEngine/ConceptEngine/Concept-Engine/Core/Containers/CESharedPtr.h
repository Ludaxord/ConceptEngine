#pragma once

#include "CEUniquePtr.h"

struct CEPtrControlBlock {
public:
	typedef uint32 RefType;

	CEPtrControlBlock() noexcept : WeakRefs(0), StrongRefs(0) {

	}

	RefType AddWeakRef() noexcept {
		return WeakRefs++;
	}

	RefType AddStrongRef() noexcept {
		return StrongRefs++;
	}

	RefType ReleaseWeakRef() noexcept {
		return WeakRefs--;
	}

	RefType ReleaseStrongRef() noexcept {
		return StrongRefs--;
	}

	RefType GetWeakReferences() const noexcept {
		return WeakRefs;
	}

	RefType GetStrongReferences() const noexcept {
		return StrongRefs;
	}

private:
	RefType WeakRefs;
	RefType StrongRefs;

};

template <typename T>
struct CETDelete {
	using TType = T;

	void operator()(TType* Ptr) noexcept {
		delete Ptr;
	}
};

template <typename T>
struct CETDelete<T[]> {
	using TType = CERemoveExtent<T>;

	void operator()(TType* Ptr) noexcept {
		delete[] Ptr;
	}
};

template <typename T, typename D>
class CEPtrBase {
public:
	template <typename TOther, typename DOther>
	friend class CEPtrBase;

	T* Get() const noexcept {
		return Ptr;
	}

	T* const* GetAddressOf() const noexcept {
		return &Ptr;
	}

	CEPtrControlBlock::RefType GetStrongReferences() const noexcept {
		return Counter ? Counter->GetStrongReferences() : 0;
	}

	CEPtrControlBlock::RefType GetWeakReferences() const noexcept {
		return Counter ? Counter->GetWeakReferences() : 0;
	}

	T* const* operator&() const noexcept {
		return GetAddressOf();
	}

	bool operator==(T* InPtr) const noexcept {
		return (Ptr == InPtr);
	}

	bool operator!=(T* InPtr) const noexcept {
		return !(*this == InPtr);
	}

	operator bool() const noexcept {
		return (Ptr != nullptr);
	}

protected:
	CEPtrBase() noexcept
		: Ptr(nullptr)
		  , Counter(nullptr) {
		static_assert(std::is_array_v<T> == std::is_array_v<D>, "Scalar types must have scalar TDelete");
		static_assert(std::is_invocable<D, T*>(), "TDelete must be a callable");
	}

	void InternalAddStrongRef() noexcept {
		// If the object has a InPtr there must be a Counter or something went wrong
		if (Ptr) {
			Assert(Counter != nullptr);
			Counter->AddStrongRef();
		}
	}

	void InternalAddWeakRef() noexcept {
		// If the object has a InPtr there must be a Counter or something went wrong
		if (Ptr) {
			Assert(Counter != nullptr);
			Counter->AddWeakRef();
		}
	}

	void InternalReleaseStrongRef() noexcept {
		// If the object has a InPtr there must be a Counter or something went wrong
		if (Ptr) {
			Assert(Counter != nullptr);
			Counter->ReleaseStrongRef();

			// When releasing the last strong reference we can destroy the pointer and counter
			if (Counter->GetStrongReferences() <= 0) {
				if (Counter->GetWeakReferences() <= 0) {
					delete Counter;
				}

				Deleter(Ptr);
				InternalClear();
			}
		}
	}

	void InternalReleaseWeakRef() noexcept {
		// If the object has a InPtr there must be a Counter or something went wrong
		if (Ptr) {
			Assert(Counter != nullptr);
			Counter->ReleaseWeakRef();

			CEPtrControlBlock::RefType StrongRefs = Counter->GetStrongReferences();
			CEPtrControlBlock::RefType WeakRefs = Counter->GetWeakReferences();
			if (WeakRefs <= 0 && StrongRefs <= 0) {
				delete Counter;
			}
		}
	}

	void InternalSwap(CEPtrBase& Other) noexcept {
		T* TempPtr = Ptr;
		CEPtrControlBlock* TempBlock = Counter;

		Ptr = Other.Ptr;
		Counter = Other.Counter;

		Other.Ptr = TempPtr;
		Other.Counter = TempBlock;
	}

	void InternalMove(CEPtrBase&& Other) noexcept {
		Ptr = Other.Ptr;
		Counter = Other.Counter;

		Other.Ptr = nullptr;
		Other.Counter = nullptr;
	}

	template <typename TOther, typename DOther>
	void InternalMove(CEPtrBase<TOther, DOther>&& Other) noexcept {
		static_assert(std::is_convertible<TOther*, T*>());

		Ptr = static_cast<TOther*>(Other.Ptr);
		Counter = Other.Counter;

		Other.Ptr = nullptr;
		Other.Counter = nullptr;
	}

	void InternalConstructStrong(T* InPtr) noexcept {
		Ptr = InPtr;
		Counter = new CEPtrControlBlock();
		InternalAddStrongRef();
	}

	template <typename TOther, typename DOther>
	void InternalConstructStrong(TOther* InPtr) noexcept {
		static_assert(std::is_convertible<TOther*, T*>());

		Ptr = static_cast<T*>(InPtr);
		Counter = new CEPtrControlBlock();
		InternalAddStrongRef();
	}

	void InternalConstructStrong(const CEPtrBase& Other) noexcept {
		Ptr = Other.Ptr;
		Counter = Other.Counter;
		InternalAddStrongRef();
	}

	template <typename TOther, typename DOther>
	void InternalConstructStrong(const CEPtrBase<TOther, DOther>& Other) noexcept {
		static_assert(std::is_convertible<TOther*, T*>());

		Ptr = static_cast<T*>(Other.Ptr);
		Counter = Other.Counter;
		InternalAddStrongRef();
	}

	template <typename TOther, typename DOther>
	void InternalConstructStrong(const CEPtrBase<TOther, DOther>& Other, T* InPtr) noexcept {
		Ptr = InPtr;
		Counter = Other.Counter;
		InternalAddStrongRef();
	}

	template <typename TOther, typename DOther>
	void InternalConstructStrong(CEPtrBase<TOther, DOther>&& Other, T* InPtr) noexcept {
		Ptr = InPtr;
		Counter = Other.Counter;

		Other.Ptr = nullptr;
		Other.Counter = nullptr;
	}

	void InternalConstructWeak(T* InPtr) noexcept {
		Ptr = InPtr;
		Counter = new CEPtrControlBlock();
		InternalAddWeakRef();
	}

	template <typename TOther>
	void InternalConstructWeak(TOther* InPtr) noexcept {
		static_assert(std::is_convertible<TOther*, T*>());

		Ptr = static_cast<T*>(InPtr);
		Counter = new CEPtrControlBlock();
		InternalAddWeakRef();
	}

	void InternalConstructWeak(const CEPtrBase& Other) noexcept {
		Ptr = Other.Ptr;
		Counter = Other.Counter;
		InternalAddWeakRef();
	}

	template <typename TOther, typename DOther>
	void InternalConstructWeak(const CEPtrBase<TOther, DOther>& Other) noexcept {
		static_assert(std::is_convertible<TOther*, T*>());

		Ptr = static_cast<T*>(Other.Ptr);
		Counter = Other.Counter;
		InternalAddWeakRef();
	}

	void InternalDestructWeak() noexcept {
		InternalReleaseWeakRef();
		InternalClear();
	}

	void InternalDestructStrong() noexcept {
		InternalReleaseStrongRef();
		InternalClear();
	}

	void InternalClear() noexcept {
		Ptr = nullptr;
		Counter = nullptr;
	}

protected:
	T* Ptr;
	CEPtrControlBlock* Counter;
	D Deleter;
};

template <typename TOther>
class CEWeakPtr;

template <typename T>
class CESharedPtr : public CEPtrBase<T, CETDelete<T>> {
	using Base = CEPtrBase<T, CETDelete<T>>;

public:
	CESharedPtr() noexcept : Base() {
	}

	CESharedPtr(std::nullptr_t) noexcept : Base() {

	}

	explicit CESharedPtr(T* Ptr) noexcept : Base() {
		Base::InternalConstructStrong(Ptr);
	}

	CESharedPtr(const CESharedPtr& Another) noexcept : Base() {
		Base::InternalConstructStrong(Another);
	}

	CESharedPtr(CESharedPtr&& Another) noexcept : Base() {
		Base::InternalMove(Move(Another));
	}

	template <typename TAnother>
	CESharedPtr(const CESharedPtr<TAnother>& Another) noexcept : Base() {
		static_assert(std::is_convertible<TAnother*, T*>());
		Base::template InternalConstructStrong<TAnother>(Another);
	}

	template <typename TAnother>
	CESharedPtr() noexcept : Base() {

	}

	template <typename TAnother>
	CESharedPtr(CESharedPtr<TAnother>&& Other) noexcept : Base() {
		static_assert(std::is_convertible<TAnother*, T*>());
		Base::template InternalMove<TAnother>(Move(Other));
	}

	template <typename TAnother>
	CESharedPtr(const CESharedPtr<TAnother>& Other, T* InPtr) noexcept : Base() {
		Base::template InternalConstructStrong<TAnother>(Other, InPtr);
	}

	template <typename TAnother>
	CESharedPtr(const CESharedPtr<TAnother>&& Other, T* InPtr) noexcept : Base() {
		Base::template InternalConstructStrong<TAnother>(Move(Other), InPtr);
	}

	template <typename TAnother>
	CESharedPtr(const CEWeakPtr<TAnother>& Another) noexcept : Base() {
		static_assert(std::is_convertible<TAnother*, T*>());
		Base::template InternalConstructStrong<TAnother>(Another);
	}

	template <typename TAnother>
	CESharedPtr(const CEUniquePtr<TAnother>&& Another) noexcept : Base() {
		static_assert(std::is_convertible<TAnother*, T*>());
		Base::template InternalConstructStrong<TAnother, CETDelete<T>>(Another.Release());
	}

	~CESharedPtr() noexcept {
		Reset();
	}

	void Reset() noexcept {
		Base::InternalDestructStrong();
	}

	void Swap(CESharedPtr& Other) noexcept {
		Base::InternalSwap(Other);
	}

	bool IsUnique() const noexcept {
		return (Base::GetStrongReferences() == 1);
	}

	T* operator->() const noexcept {
		return Base::Get();
	}

	T& operator*() const noexcept {
		Assert(Base::Ptr != nullptr);
		return *Base::Ptr;
	}

	CESharedPtr& operator=(const CESharedPtr& Other) noexcept {
		CESharedPtr(Other).Swap(*this);
		return *this;
	}

	CESharedPtr& operator=(CESharedPtr&& Other) noexcept {
		CESharedPtr(Move(Other)).Swap(*this);
		return *this;
	}

	template <typename TOther>
	CESharedPtr& operator=(const CESharedPtr<TOther>& Other) noexcept {
		CESharedPtr(Other).Swap(*this);
		return *this;
	}

	template <typename TOther>
	CESharedPtr& operator=(CESharedPtr<TOther>&& Other) noexcept {
		CESharedPtr(Move(Other)).Swap(*this);
		return *this;
	}

	CESharedPtr& operator=(T* InPtr) noexcept {
		if (Base::Ptr != InPtr) {
			Reset();
			Base::InternalConstructStrong(InPtr);
		}

		return *this;
	}

	CESharedPtr& operator=(std::nullptr_t) noexcept {
		Reset();
		return *this;
	}

	bool operator==(const CESharedPtr& Other) const noexcept {
		return (Base::Ptr == Other.Ptr);
	}

	bool operator!=(const CESharedPtr& Other) const noexcept {
		return !(*this == Other);
	}
};

template <typename T>
class CESharedPtr<T[]> : public CEPtrBase<T, CETDelete<T[]>> {
	using Base = CEPtrBase<T, CETDelete<T[]>>;

public:
	CESharedPtr() noexcept : Base() {

	}

	CESharedPtr(std::nullptr_t) noexcept : Base() {

	}

	explicit CESharedPtr(T* Ptr) noexcept : Base() {
		Base::InternalConstructStrong(Ptr);
	}

	CESharedPtr(const CESharedPtr& Other) noexcept : Base() {
		Base::InternalConstructStrong(Other);
	}

	CESharedPtr(CESharedPtr&& Other) noexcept : Base() {
		Base::InternalMove(Move(Other));
	}

	template <typename TOther>
	CESharedPtr(const CESharedPtr<TOther[]>& Other) noexcept : Base() {
		static_assert(std::is_convertible<TOther*, T*>());
		Base::template InternalConstructStrong<TOther>(Other);
	}

	template <typename TOther>
	CESharedPtr(const CESharedPtr<TOther[]>& Other, T* InPtr) noexcept : Base() {
		Base::template InternalConstructStrong<TOther>(Move(Other), InPtr);
	}

	template <typename TOther>
	CESharedPtr(CESharedPtr<TOther[]>&& Other, T* InPtr) noexcept : Base() {
		Base::template InternalConstructStrong<TOther>(Move(Other), InPtr);
	}

	template <typename TOther>
	CESharedPtr(CESharedPtr<TOther[]>&& Other) noexcept : Base() {
		static_assert(std::is_convertible<TOther*, T*>());
		Base::template InternalMove<TOther>(Move(Other));
	}

	template <typename TOther>
	explicit CESharedPtr(const CEWeakPtr<TOther[]>& Other) noexcept : Base() {
		static_assert(std::is_convertible<TOther*, T*>());
		Base::template InternalConstructStrong<TOther>(Other);
	}

	template <typename TOther>
	CESharedPtr(CEUniquePtr<TOther[]>&& Other) noexcept : Base() {
		static_assert(std::is_convertible<TOther*, T*>());
		Base::template InternalConstructStrong<TOther>(Other.Release());
	}

	~CESharedPtr() {
		Reset();
	}

	void Reset() noexcept {
		Base::InternalDestructStrong();
	}

	void Swap(CESharedPtr& Other) noexcept {
		Base::InternalSwap(Other);
	}

	bool IsUnique() const noexcept {
		return (Base::GetStrongReferences() == 1);
	}

	T& operator[](uint32 Index) noexcept {
		Assert(Base::Ptr != nullptr);;
		return Base::Ptr[Index];
	}

	CESharedPtr& operator=(const CESharedPtr& Other) noexcept {
		CESharedPtr(Other).Swap(*this);
		return *this;
	}

	CESharedPtr& operator=(CESharedPtr&& Other) noexcept {
		CESharedPtr(Move(Other)).Swap(*this);
		return *this;
	}

	template <typename TOther>
	CESharedPtr& operator=(const CESharedPtr<TOther[]>& Other) noexcept {
		CESharedPtr(Other).Swap(*this);
		return *this;
	}

	template <typename TOther>
	CESharedPtr& operator=(CESharedPtr<TOther[]>&& Other) noexcept {
		CESharedPtr(Move(Other)).Swap(*this);
		return *this;
	}

	CESharedPtr& operator=(T* InPtr) noexcept {
		if (this->Ptr != InPtr) {
			Reset();
			Base::InternalConstructStrong(InPtr);
		}

		return *this;
	}

	CESharedPtr& operator=(std::nullptr_t) noexcept {
		Reset();
		return *this;
	}

	bool operator==(const CESharedPtr& Other) const noexcept {
		return (Base::Ptr == Other.Ptr);
	}

	bool operator!=(const CESharedPtr& Other) const noexcept {
		return !(*this == Other);
	}
};


template <typename T>
class CEWeakPtr : public CEPtrBase<T, CETDelete<T>> {
	using Base = CEPtrBase<T, CETDelete<T>>;

public:
	CEWeakPtr() noexcept : Base() {

	}

	CEWeakPtr(const CESharedPtr<T>& Other) noexcept
		: Base() {
		Base::InternalConstructWeak(Other);
	}

	template <typename TOther>
	CEWeakPtr(const CESharedPtr<TOther>& Other) noexcept
		: Base() {
		static_assert(std::is_convertible<TOther*, T*>(), "TWeakPtr: Trying to convert non-convertable types");
		Base::template InternalConstructWeak<TOther>(Other);
	}

	CEWeakPtr(const CEWeakPtr& Other) noexcept
		: Base() {
		Base::InternalConstructWeak(Other);
	}

	CEWeakPtr(CEWeakPtr&& Other) noexcept
		: Base() {
		Base::InternalMove(Move(Other));
	}

	template <typename TOther>
	CEWeakPtr(const CEWeakPtr<TOther>& Other) noexcept
		: Base() {
		static_assert(std::is_convertible<TOther*, T*>(), "TWeakPtr: Trying to convert non-convertable types");
		Base::template InternalConstructWeak<TOther>(Other);
	}

	template <typename TOther>
	CEWeakPtr(CEWeakPtr<TOther>&& Other) noexcept
		: Base() {
		static_assert(std::is_convertible<TOther*, T*>(), "TWeakPtr: Trying to convert non-convertable types");
		Base::template InternalMove<TOther>(Move(Other));
	}

	~CEWeakPtr() {
		Reset();
	}

	void Reset() noexcept {
		Base::InternalDestructWeak();
	}

	void Swap(CEWeakPtr& Other) noexcept {
		Base::InternalSwap(Other);
	}

	bool IsExpired() const noexcept {
		return (Base::GetStrongReferences() < 1);
	}

	CEWeakPtr<T> MakeShared() noexcept {
		const CEWeakPtr& This = *this;
		return Move(CESharedPtr<T>(This));
	}

	T* operator->() const noexcept {
		return Base::Get();
	}

	T& operator*() const noexcept {
		Assert(Base::Ptr != nullptr);
		return *Base::Ptr;
	}

	CEWeakPtr& operator=(const CEWeakPtr& Other) noexcept {
		CEWeakPtr(Other).Swap(*this);
		return *this;
	}

	CEWeakPtr& operator=(CEWeakPtr&& Other) noexcept {
		CEWeakPtr(Move(Other)).Swap(*this);
		return *this;
	}

	template <typename TOther>
	CEWeakPtr& operator=(const CEWeakPtr<TOther>& Other) noexcept {
		CEWeakPtr(Other).Swap(*this);
		return *this;
	}

	template <typename TOther>
	CEWeakPtr& operator=(CEWeakPtr<TOther>&& Other) noexcept {
		CEWeakPtr(Move(Other)).Swap(*this);
		return *this;
	}

	CEWeakPtr& operator=(T* InPtr) noexcept {
		if (Base::Ptr != InPtr) {
			Reset();
			Base::InternalConstructWeak(InPtr);
		}

		return *this;
	}

	CEWeakPtr& operator=(std::nullptr_t) noexcept {
		Reset();
		return *this;
	}

	bool operator==(const CEWeakPtr& Other) const noexcept {
		return (Base::Ptr == Other.Ptr);
	}

	bool operator!=(const CEWeakPtr& Other) const noexcept {
		return !(*this == Other);
	}
};

template <typename T>
class CEWeakPtr<T[]> : public CEPtrBase<T, CETDelete<T[]>> {
	using Base = CEPtrBase<T, CETDelete<T[]>>;

public:
	CEWeakPtr() noexcept
		: Base() {
	}

	CEWeakPtr(const CESharedPtr<T>& Other) noexcept
		: Base() {
		Base::InternalConstructWeak(Other);
	}

	template <typename TOther>
	CEWeakPtr(const CESharedPtr<TOther[]>& Other) noexcept
		: Base() {
		static_assert(std::is_convertible<TOther*, T*>(), "TWeakPtr: Trying to convert non-convertable types");
		Base::template InternalConstructWeak<TOther>(Other);
	}

	CEWeakPtr(const CEWeakPtr& Other) noexcept
		: Base() {
		Base::InternalConstructWeak(Other);
	}

	CEWeakPtr(CEWeakPtr&& Other) noexcept
		: Base() {
		Base::InternalMove(Move(Other));
	}

	template <typename TOther>
	CEWeakPtr(const CEWeakPtr<TOther[]>& Other) noexcept
		: Base() {
		static_assert(std::is_convertible<TOther*, T*>(), "TWeakPtr: Trying to convert non-convertable types");
		Base::template InternalConstructWeak<TOther>(Other);
	}

	template <typename TOther>
	CEWeakPtr(CEWeakPtr<TOther[]>&& Other) noexcept
		: Base() {
		static_assert(std::is_convertible<TOther*, T*>(), "TWeakPtr: Trying to convert non-convertable types");
		Base::template InternalMove<TOther>(Move(Other));
	}

	~CEWeakPtr() {
		Reset();
	}

	void Reset() noexcept {
		Base::InternalDestructWeak();
	}

	void Swap(CEWeakPtr& Other) noexcept {
		Base::InternalSwap(Other);
	}

	bool IsExpired() const noexcept {
		return (Base::GetStrongReferences() < 1);
	}

	CESharedPtr<T[]> MakeShared() noexcept {
		const CEWeakPtr& This = *this;
		return Move(CESharedPtr<T[]>(This));
	}

	T& operator[](uint32 Index) noexcept {
		Assert(Base::Ptr != nullptr);
		return Base::Ptr[Index];
	}

	CEWeakPtr& operator=(const CEWeakPtr& Other) noexcept {
		CEWeakPtr(Other).Swap(*this);
		return *this;
	}

	CEWeakPtr& operator=(CEWeakPtr&& Other) noexcept {
		CEWeakPtr(Move(Other)).Swap(*this);
		return *this;
	}

	template <typename TOther>
	CEWeakPtr& operator=(const CEWeakPtr<TOther[]>& Other) noexcept {
		CEWeakPtr(Other).Swap(*this);
		return *this;
	}

	template <typename TOther>
	CEWeakPtr& operator=(CEWeakPtr<TOther[]>&& Other) noexcept {
		CEWeakPtr(Move(Other)).Swap(*this);
		return *this;
	}

	CEWeakPtr& operator=(T* InPtr) noexcept {
		if (Base::Ptr != InPtr) {
			Reset();
			Base::InternalConstructWeak(InPtr);
		}

		return *this;
	}

	CEWeakPtr& operator=(std::nullptr_t) noexcept {
		Reset();
		return *this;
	}

	bool operator==(const CEWeakPtr& Other) const noexcept {
		return (Base::Ptr == Other.Ptr);
	}

	bool operator!=(const CEWeakPtr& Other) const noexcept {
		return !(*this == Other);
	}
};

template <typename T, typename... TArgs>
CEEnableIf<!CEIsArray<T>, CESharedPtr<T>> MakeShared(TArgs&&... Args) noexcept {
	T* RefCountedPtr = new T(Forward<TArgs>(Args)...);
	return Move(CESharedPtr<T>(RefCountedPtr));
}

template <typename T>
CEEnableIf<CEIsArray<T>, CESharedPtr<T>> MakeShared(uint32 Size) noexcept {
	using TType = CERemoveExtent<T>;

	TType* RefCountedPtr = new TType[Size];
	return Move(CESharedPtr<T>(RefCountedPtr));
}

template <typename T0, typename T1>
CEEnableIf<CEIsArray<T0> == CEIsArray<T1>, CESharedPtr<T0>> StaticCast(const CESharedPtr<T1>& Pointer) noexcept {
	using TType = CERemoveExtent<T0>;

	TType* RawPointer = static_cast<TType*>(Pointer.Get());
	return Move(CESharedPtr<T0>(Pointer, RawPointer));
}

template <typename T0, typename T1>
CEEnableIf<CEIsArray<T0> == CEIsArray<T1>, CESharedPtr<T0>> StaticCast(CESharedPtr<T1>&& Pointer)
noexcept {
	using TType = CERemoveExtent<T0>;

	TType* RawPointer = static_cast<TType*>(Pointer.Get());
	return Move(CESharedPtr<T0>(Move(Pointer), RawPointer));
}

// const_cast
template <typename T0, typename T1>
CEEnableIf<CEIsArray<T0> == CEIsArray<T1>, CESharedPtr<T0>> ConstCast(const CESharedPtr<T1>& Pointer) noexcept {
	using TType = CERemoveExtent<T0>;

	TType* RawPointer = const_cast<TType*>(Pointer.Get());
	return Move(CESharedPtr<T0>(Pointer, RawPointer));
}

template <typename T0, typename T1>
CEEnableIf<CEIsArray<T0> == CEIsArray<T1>, CESharedPtr<T0>> ConstCast(CESharedPtr<T1>&& Pointer)
noexcept {
	using TType = CERemoveExtent<T0>;

	TType* RawPointer = const_cast<TType*>(Pointer.Get());
	return Move(CESharedPtr<T0>(Move(Pointer), RawPointer));
}

// reinterpret_cast
template <typename T0, typename T1>
CEEnableIf<CEIsArray<T0> == CEIsArray<T1>, CESharedPtr<T0>> ReinterpretCast(const CESharedPtr<T1>& Pointer) noexcept {
	using TType = CERemoveExtent<T0>;

	TType* RawPointer = reinterpret_cast<TType*>(Pointer.Get());
	return Move(CESharedPtr<T0>(Pointer, RawPointer));
}

template <typename T0, typename T1>
CEEnableIf<CEIsArray<T0> == CEIsArray<T1>, CESharedPtr<T0>> ReinterpretCast(CESharedPtr<T1>&& Pointer)
noexcept {
	using TType = CERemoveExtent<T0>;

	TType* RawPointer = reinterpret_cast<TType*>(Pointer.Get());
	return Move(CESharedPtr<T0>(Move(Pointer), RawPointer));
}

// dynamic_cast
template <typename T0, typename T1>
CEEnableIf<CEIsArray<T0> == CEIsArray<T1>, CESharedPtr<T0>> DynamicCast(const CESharedPtr<T1>& Pointer) noexcept {
	using TType = CERemoveExtent<T0>;

	TType* RawPointer = dynamic_cast<TType*>(Pointer.Get());
	return Move(CESharedPtr<T0>(Pointer, RawPointer));
}

template <typename T0, typename T1>
CEEnableIf<CEIsArray<T0> == CEIsArray<T1>, CESharedPtr<T0>> DynamicCast(CESharedPtr<T1>&& Pointer)
noexcept {
	using TType = CERemoveExtent<T0>;

	TType* RawPointer = dynamic_cast<TType*>(Pointer.Get());
	return Move(CESharedPtr<T0>(Move(Pointer), RawPointer));
}
