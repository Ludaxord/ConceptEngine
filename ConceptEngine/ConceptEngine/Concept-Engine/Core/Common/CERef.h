#pragma once
#include <type_traits>

#include "../Containers/CEContainerUtilities.h"

namespace ConceptEngine::Core::Common {

	template <typename T>
	class CERef {
	public:
		template <typename TOther>
		friend class CERef;

		CERef() noexcept
			: RefPtr(nullptr) {
		}

		CERef(const CERef& Other) noexcept
			: RefPtr(Other.RefPtr) {
			AddRef();
		}

		template <typename TOther>
		CERef(const CERef<TOther>& Other) noexcept
			: RefPtr(Other.RefPtr) {
			static_assert(std::is_convertible<TOther*, T*>());
			AddRef();
		}

		CERef(CERef&& Other) noexcept
			: RefPtr(Other.RefPtr) {
			Other.RefPtr = nullptr;
		}

		template <typename TOther>
		CERef(CERef<TOther>&& Other) noexcept
			: RefPtr(Other.RefPtr) {
			static_assert(std::is_convertible<TOther*, T*>());
			Other.RefPtr = nullptr;
		}

		CERef(T* InPtr) noexcept
			: RefPtr(InPtr) {
		}

		template <typename TOther>
		CERef(TOther* InPtr) noexcept
			: RefPtr(InPtr) {
			static_assert(std::is_convertible<TOther*, T*>());
		}

		~CERef() {
			Release();
		}

		T* Reset() noexcept {
			T* WeakPtr = RefPtr;
			Release();

			return WeakPtr;
		}

		T* ReleaseOwnership() noexcept {
			T* WeakPtr = RefPtr;
			RefPtr = nullptr;
			return WeakPtr;
		}

		void AddRef() noexcept {
			if (RefPtr) {
				RefPtr->AddRef();
			}
		}

		void Swap(T* InPtr) noexcept {
			Release();
			RefPtr = InPtr;
		}

		template <typename TOther>
		void Swap(TOther* InPtr) noexcept {
			static_assert(std::is_convertible<TOther*, T*>());

			Release();
			RefPtr = InPtr;
		}

		T* Get() const noexcept {
			return RefPtr;
		}

		T* GetRefCount() const noexcept {
			return RefPtr->GetRefCount();
		}

		T* GetAndAddRef() noexcept {
			AddRef();
			return RefPtr;
		}

		template <typename TCastType>
		Containers::CEEnableIf<std::is_convertible_v<TCastType*, T*>, TCastType*> GetAs() const noexcept {
			return static_cast<TCastType*>(RefPtr);
		}

		T* const* GetAddressOf() const noexcept {
			return &RefPtr;
		}

		T& Dereference() const noexcept {
			return *RefPtr;
		}

		T* operator->() const noexcept {
			return Get();
		}

		T* const* operator&() const noexcept {
			return GetAddressOf();
		}

		T& operator*() const noexcept {
			return Dereference();
		}

		bool operator==(T* InPtr) const noexcept {
			return (RefPtr == InPtr);
		}

		bool operator==(const CERef& Other) const noexcept {
			return (RefPtr == Other.RefPtr);
		}

		bool operator!=(T* InPtr) const noexcept {
			return (RefPtr != InPtr);
		}

		bool operator!=(const CERef& Other) const noexcept {
			return (RefPtr != Other.RefPtr);
		}

		bool operator==(std::nullptr_t) const noexcept {
			return (RefPtr == nullptr);
		}

		bool operator!=(std::nullptr_t) const noexcept {
			return (RefPtr != nullptr);
		}

		template <typename TOther>
		Containers::CEEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator==(TOther* RHS) const noexcept {
			return (RefPtr == RHS);
		}

		template <typename TOther>
		friend Containers::CEEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator==(
			TOther* LHS, const CERef& RHS) noexcept {
			return (RHS == LHS);
		}

		template <typename TOther>
		Containers::CEEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator==(
			const CERef<TOther>& RHS) const noexcept {
			return (RefPtr == RHS.RefPtr);
		}

		template <typename TOther>
		Containers::CEEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator!=(TOther* RHS) const noexcept {
			return (RefPtr != RHS);
		}

		template <typename TOther>
		friend Containers::CEEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator!=(
			TOther* LHS, const CERef& RHS) noexcept {
			return (RHS != LHS);
		}

		template <typename TOther>
		Containers::CEEnableIf<std::is_convertible_v<TOther*, T*>, bool> operator!=(
			const CERef<TOther>& RHS) const noexcept {
			return (RefPtr != RHS.RefPtr);
		}

		operator bool() const noexcept {
			return (RefPtr != nullptr);
		}

		CERef& operator=(const CERef& Other) noexcept {
			if (this != std::addressof(Other)) {
				Release();

				RefPtr = Other.RefPtr;
				AddRef();
			}

			return *this;
		}

		template <typename TOther>
		CERef& operator=(const CERef<TOther>& Other) noexcept {
			static_assert(std::is_convertible<TOther*, T*>());

			if (this != std::addressof(Other)) {
				Release();

				RefPtr = Other.RefPtr;
				AddRef();
			}

			return *this;
		}

		CERef& operator=(CERef&& Other) noexcept {
			if (this != std::addressof(Other)) {
				Release();

				RefPtr = Other.RefPtr;
				Other.RefPtr = nullptr;
			}

			return *this;
		}

		template <typename TOther>
		CERef& operator=(CERef<TOther>&& Other) noexcept {
			static_assert(std::is_convertible<TOther*, T*>());

			if (this != std::addressof(Other)) {
				Release();

				RefPtr = Other.RefPtr;
				Other.RefPtr = nullptr;
			}

			return *this;
		}

		CERef& operator=(T* InPtr) noexcept {
			if (RefPtr != InPtr) {
				Release();
				RefPtr = InPtr;
			}

			return *this;
		}

		template <typename TOther>
		CERef& operator=(TOther* InPtr) noexcept {
			static_assert(std::is_convertible<TOther*, T*>());

			if (RefPtr != InPtr) {
				Release();
				RefPtr = InPtr;
			}

			return *this;
		}

		CERef& operator=(std::nullptr_t) noexcept {
			Release();
			return *this;
		}

	private:
		void Release() noexcept {
			if (RefPtr) {
				RefPtr->Release();
				RefPtr = nullptr;
			}
		}

		T* RefPtr;
	};

	// static_cast
	template <typename T, typename U>
	CERef<T> StaticCast(const CERef<U>& Pointer) {
		T* RawPointer = static_cast<T*>(Pointer.Get());
		RawPointer->AddRef();
		return CERef<T>(RawPointer);
	}

	template <typename T, typename U>
	CERef<T> StaticCast(CERef<U>&& Pointer) {
		T* RawPointer = static_cast<T*>(Pointer.Get());
		return CERef<T>(RawPointer);
	}

	// const_cast
	template <typename T, typename U>
	CERef<T> ConstCast(const CERef<U>& Pointer) {
		T* RawPointer = const_cast<T*>(Pointer.Get());
		RawPointer->AddRef();
		return CERef<T>(RawPointer);
	}

	template <typename T, typename U>
	CERef<T> ConstCast(CERef<U>&& Pointer) {
		T* RawPointer = const_cast<T*>(Pointer.Get());
		return CERef<T>(RawPointer);
	}

	// reinterpret_cast
	template <typename T, typename U>
	CERef<T> ReinterpretCast(const CERef<U>& Pointer) {
		T* RawPointer = reinterpret_cast<T*>(Pointer.Get());
		RawPointer->AddRef();
		return CERef<T>(RawPointer);
	}

	template <typename T, typename U>
	CERef<T> ReinterpretCast(CERef<U>&& Pointer) {
		T* RawPointer = reinterpret_cast<T*>(Pointer.Get());
		return CERef<T>(RawPointer);
	}

	// dynamic_cast
	template <typename T, typename U>
	CERef<T> DynamicCast(const CERef<U>& Pointer) {
		T* RawPointer = dynamic_cast<T*>(Pointer.Get());
		RawPointer->AddRef();
		return CERef<T>(RawPointer);
	}

	template <typename T, typename U>
	CERef<T> DynamicCast(CERef<U>&& Pointer) {
		T* RawPointer = dynamic_cast<T*>(Pointer.Get());
		return CERef<T>(RawPointer);
	}

	// Converts a raw pointer into a CERef
	template <typename T, typename U>
	CERef<T> MakeSharedRef(U* InRefCountedObject) {
		if (InRefCountedObject) {
			InRefCountedObject->AddRef();
			return CERef<T>(static_cast<T*>(InRefCountedObject));
		}

		return CERef<T>();
	}
}
