#pragma once
namespace ConceptEngine::Core::Delegates {
	template <typename TInvokable>
	class CEDelegateBase;

	template <typename TReturn, typename... TArgs>
	class CEDelegateBase<TReturn(TArgs ...)> {

	};
}
