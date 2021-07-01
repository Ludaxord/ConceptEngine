#pragma once
#include <cstddef>
#include <type_traits>

#include "CEContainerUtilities.h"
#include "../Common/CETypes.h"

template <typename T>
class CEUniquePtr {
public:
	template <typename TOther>
	friend class CEUniquePtr;

	CEUniquePtr(const CEUniquePtr& Other) = delete;
	CEUniquePtr& operator=(const CEUniquePtr& Other) noexcept = delete;

	CEUniquePtr() noexcept: Ptr(nullptr) {
	}

	CEUniquePtr(std::nullptr_t) noexcept : Ptr(nullptr) {
	}

	explicit CEUniquePtr(T* ptr) noexcept : Ptr(ptr) {
	}

	CEUniquePtr(CEUniquePtr&& Other) noexcept
		: Ptr(Other.Ptr) {
		Other.Ptr = nullptr;
	}

	template <typename TOther>
	CEUniquePtr(CEUniquePtr<TOther>&& Other) noexcept
		: Ptr(Other.Ptr) {
		static_assert(std::is_convertible<TOther*, T*>());
		Other.Ptr = nullptr;
	}

	~CEUniquePtr() {
		Reset();
	}

	T* Release() noexcept {
		T* WeakPtr = Ptr;
		Ptr = nullptr;
		return WeakPtr;
	}

	void Reset() noexcept {
		InternalRelease();
		Ptr = nullptr;
	}

	void Swap(CEUniquePtr& Other) noexcept {
		T* TempPtr = Ptr;
		Ptr = Other.Ptr;
		Other.Ptr = TempPtr;
	}

	T* Get() const noexcept {
		return Ptr;
	}

	T* const* GetAddressOf() const noexcept {
		return &Ptr;
	}

	T* operator->() const noexcept {
		return Get();
	}

	T& operator*() const noexcept {
		return (*Ptr);
	}

	T* const* operator&() const noexcept {
		return GetAddressOf();
	}

	CEUniquePtr& operator=(T* InPtr) noexcept {
		if (Ptr != InPtr) {
			Reset();
			Ptr = InPtr;
		}

		return *this;
	}

	CEUniquePtr& operator=(CEUniquePtr&& Other) noexcept {
		if (this != std::addressof(Other)) {
			Reset();
			Ptr = Other.Ptr;
			Other.Ptr = nullptr;
		}

		return *this;
	}

	template <typename TOther>
	CEUniquePtr& operator=(CEUniquePtr<TOther>&& Other) noexcept {
		static_assert(std::is_convertible<TOther*, T*>());

		if (this != std::addressof(Other)) {
			Reset();
			Ptr = Other.Ptr;
			Other.Ptr = nullptr;
		}

		return *this;
	}

	CEUniquePtr& operator=(std::nullptr_t) noexcept {
		Reset();
		return *this;
	}

	bool operator==(const CEUniquePtr& Other) const noexcept {
		return (Ptr == Other.Ptr);
	}

	bool operator!=(const CEUniquePtr& Other) const noexcept {
		return !(*this == Other);
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

private:
	void InternalRelease() noexcept {
		if (Ptr) {
			delete Ptr;
			Ptr = nullptr;
		}
	}

	T* Ptr;
};


template <typename T>
class CEUniquePtr<T[]> {
public:
	template <typename TOther>
	friend class TUniquePtr;

	CEUniquePtr(const CEUniquePtr& Other) = delete;
	CEUniquePtr& operator=(const CEUniquePtr& Other) noexcept = delete;

	CEUniquePtr() noexcept
		: Ptr(nullptr) {
	}

	CEUniquePtr(std::nullptr_t) noexcept
		: Ptr(nullptr) {
	}

	explicit CEUniquePtr(T* InPtr) noexcept
		: Ptr(InPtr) {
	}

	CEUniquePtr(CEUniquePtr&& Other) noexcept
		: Ptr(Other.Ptr) {
		Other.Ptr = nullptr;
	}

	template <typename TOther>
	CEUniquePtr(CEUniquePtr<TOther>&& Other) noexcept
		: Ptr(Other.Ptr) {
		static_assert(std::is_convertible<TOther*, T*>());
		Other.Ptr = nullptr;
	}

	~CEUniquePtr() {
		Reset();
	}

	T* Release() noexcept {
		T* WeakPtr = Ptr;
		Ptr = nullptr;
		return WeakPtr;
	}

	void Reset() noexcept {
		InternalRelease();
		Ptr = nullptr;
	}

	void Swap(CEUniquePtr& Other) noexcept {
		T* TempPtr = Ptr;
		Ptr = Other.Ptr;
		Other.Ptr = TempPtr;
	}

	T* Get() const noexcept {
		return Ptr;
	}

	T* const* GetAddressOf() const noexcept {
		return &Ptr;
	}

	T* const* operator&() const noexcept {
		return GetAddressOf();
	}

	T& operator[](uint32 Index) noexcept {
		Assert(Ptr != nullptr);
		return Ptr[Index];
	}

	CEUniquePtr& operator=(T* InPtr) noexcept {
		if (Ptr != InPtr) {
			Reset();
			Ptr = InPtr;
		}

		return *this;
	}

	CEUniquePtr& operator=(CEUniquePtr&& Other) noexcept {
		if (this != std::addressof(Other)) {
			Reset();
			Ptr = Other.Ptr;
			Other.Ptr = nullptr;
		}

		return *this;
	}

	template <typename TOther>
	CEUniquePtr& operator=(CEUniquePtr<TOther>&& Other) noexcept {
		static_assert(std::is_convertible<TOther*, T*>());

		if (this != std::addressof(Other)) {
			Reset();
			Ptr = Other.Ptr;
			Other.Ptr = nullptr;
		}

		return *this;
	}

	CEUniquePtr& operator=(std::nullptr_t) noexcept {
		Reset();
		return *this;
	}

	bool operator==(const CEUniquePtr& Other) const noexcept {
		return (Ptr == Other.Ptr);
	}

	bool operator!=(const CEUniquePtr& Other) const noexcept {
		return !(*this == Other);
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

private:
	void InternalRelease() noexcept {
		if (Ptr) {
			delete Ptr;
			Ptr = nullptr;
		}
	}

	T* Ptr;
};

// MakeUnique - Creates a new object together with a UniquePtr

template <typename T, typename... TArgs>
CEEnableIf<!CEIsArray<T>, CEUniquePtr<T>> MakeUnique(TArgs&&... Args) noexcept {
	T* UniquePtr = new T(Forward<TArgs>(Args)...);
	return Move(CEUniquePtr<T>(UniquePtr));
}

template <typename T>
CEEnableIf<CEIsArray<T>, CEUniquePtr<T>> MakeUnique(uint32 Size) noexcept {
	using TType = CERemoveExtent<T>;

	TType* UniquePtr = new TType[Size];
	return Move(CEUniquePtr<T>(UniquePtr));
}
