#pragma once
#include "CEMulticastBase.h"

namespace ConceptEngine::Core::Delegates {
	template <typename... TArgs>
	class CEMulticastDelegateBase : public CEMulticastBase<TArgs...> {
	protected:
		typedef CEMulticastBase<TArgs...> Base;
		typedef typename Base::IDelegate IDelegate;
	public:
		CEMulticastDelegateBase(): Base() {

		}

		CEMulticastDelegateBase(const CEMulticastDelegateBase& Another) : Base() {
			for (IDelegate* Delegate : Another.Delegates) {
				Assert(Delegate != nullptr);
				Base::Delegates.EmplaceBack(Delegate->Clone());
			}
		}

		CEMulticastDelegateBase(CEMulticastDelegateBase&& Another) : Base() {
			Base::Delegates = Containers::Move(Another.Delegates);
		}

		~CEMulticastDelegateBase() {
			UnbindAll();
		}

		void UnbindAll() {
			for (IDelegate* Delegate : Base::Delegates) {
				Assert(Delegate != nullptr);
				delete Delegate;
			}
		}

		void Swap(CEMulticastDelegateBase& Another) {
			CEMulticastDelegateBase Temp(Containers::Move(*this));
			Base::Delegates = Containers::Move(Another.Delegates);
			Another.Delegates = Containers::Move(Temp.Delegates);
		}

		CEMulticastDelegateBase& operator=(const CEMulticastDelegateBase& RHS) {
			CEMulticastDelegateBase(RHS).Swap(*this);
			return *this;
		}

		CEMulticastDelegateBase& operator=(CEMulticastDelegateBase&& RHS) {
			CEMulticastDelegateBase(Containers::Move(RHS)).Swap(*this);
			return *this;
		}
	};

	template <typename... TArgs>
	class CEMulticastDelegate : public CEMulticastDelegateBase<TArgs...> {
		typedef CEMulticastDelegateBase<TArgs...> Base;
		typedef typename Base::IDelegate IDelegate;
	public:
		void Broadcast(TArgs ... Args) {
			for (IDelegate* Delegate : Base::Delegates) {
				Assert(Delegate != nullptr);
				Delegate->Execute(Containers::Forward<TArgs>(Args)...);
			}
		}

		void operator()(TArgs ... Args) {
			return Broadcast(Containers::Forward<TArgs>(Args)...);
		}
	};

	template <>
	class CEMulticastDelegate<void> : public CEMulticastDelegateBase<void> {
		typedef CEMulticastDelegateBase<void> Base;
		typedef typename Base::IDelegate IDelegate;

	public:
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
