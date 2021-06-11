#pragma once
#include "CEMulticastBase.h"

namespace ConceptEngine::Core::Delegates {

	template <typename... TArgs>
	class CEEventBase : public CEMulticastBase<TArgs...> {
	protected:
		typedef CEMulticastBase<TArgs...> Base;
		typedef typename Base::IDelegate IDelegate;

		CEEventBase() : Base() {
		}

		CEEventBase(const CEEventBase& Another) : Base() {
			for (IDelegate* Delegate : Another.Delegates) {
				Assert(Delegate != nullptr);
				Base::Delegates.EmplaceBack(Delegate->Clone());
			}
		}

		CEEventBase(CEEventBase&& Another) : Base() {
			Base::Delegates = Move(Another.Delegates);
		}

		~CEEventBase() {
			UnbindAll();
		}

		void UnbindAll() {
			for (IDelegate* Delegate : Base::Delegates) {
				Assert(Delegate != nullptr);
				delete Delegate;
			}

			Base::Delegates.Clear();
		}

		void Swap(CEEventBase& Another) {
			CEEventBase Temp(Move(*this));
			Base::Delegates = Move(Another.Delegates);
			Another.Delegates = Move(Temp.Delegates);
		}

		CEEventBase& operator=(const CEEventBase& RHS) {
			CEEventBase(RHS).Swap(*this);
			return *this;
		}

		CEEventBase& operator=(CEEventBase&& RHS) {
			CEEventBase(Move(RHS)).Swap(*this);
			return *this;
		}
	};

	template <typename... TArgs>
	class CEEvent : public CEEventBase<TArgs...> {
	protected:
		typedef CEEventBase<TArgs...> Base;
		typedef typename Base::IDelegate IDelegate;

		void Broadcast(TArgs ... Args) {
			for (IDelegate* Delegate : Base::Delegates) {
				Assert(Delegate != nullptr);
				Delegate->Execute(Forward<TArgs>(Args)...);
			}
		}

		void operator()(TArgs ... Args) {
			return Broadcast(Forward<TArgs>(Args)...);
		}
	};

	template <>
	class CEEvent<void> : public CEEventBase<void> {
	protected:
		typedef CEEventBase<void> Base;
		typedef typename Base::IDelegate IDelegate;

		void Broadcast() {
			for (IDelegate* Delegate : Base::Delegates) {
				Assert(Delegate != nullptr);
				Delegate->Execute();
			}
		}

		void operator()() {
			return Broadcast();
		}
	};

}

#define DECLARE_EVENT(EventType, EventDispatcherType, ...) \
    class EventType : public ConceptEngine::Core::Delegates::CEEvent<__VA_ARGS__> \
    { \
        friend class EventDispatcherType; \
    };
