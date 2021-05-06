#pragma once
namespace ConceptEngine::Core::Containers {

	/**
	 * CERemoveReference - Removes reference and retrive types
	 */
	template <typename T>
	struct _CERemoveReference {
		using TType = T;
	};

	template <typename T>
	struct _CERemoveReference<T&> {
		using TType = T;
	};

	template <typename T>
	struct _CERemoveReference<T&&> {
		using TType = T;
	};

	template <typename T>
	using CERemoveReference = typename _CERemoveReference<T>::TType;

	/**
	 * Move
	 * Move an object by converting it into a rValue
	 */
	template <typename T>
	constexpr CERemoveReference<T> Move(T&& Arg) noexcept {
		return static_cast<CERemoveReference<T>&&>(Arg);
	}

	/**
	 * Forward
	 * Forward an object by converting it into a rValue from lValue
	 */
	template <typename T>
	constexpr T&& Forward(CERemoveReference<T>& Arg) noexcept {
		return static_cast<T&&>(Arg);
	}

	template <typename T>
	constexpr T&& Forward(CERemoveReference<T>&& Arg) noexcept {
		return static_cast<T&&>(Arg);
	}
}
