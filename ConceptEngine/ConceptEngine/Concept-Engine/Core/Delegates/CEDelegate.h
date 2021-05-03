#pragma once
#include "CEDelegateBase.h"

namespace ConceptEngine::Core::Delegates {
	template <typename TInvokable>
	class CEDelegate;

	template <typename TReturn, typename... TArgs>
	class CEDelegate<TReturn(TArgs ...)> : private CEDelegateBase<TReturn(TArgs ...)> {

	};
}
