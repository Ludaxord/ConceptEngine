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
	 * CEEnableIf
	 */

	template <bool B, typename T = void>
	struct _CEEnableIf {

	};

	template <typename T>
	struct _CEEnableIf<true, T> {
		using CEType = T;
	};

	template <bool B, typename T = void>
	using CEEnableIf = typename _CEEnableIf<B, T>::CEType;

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
