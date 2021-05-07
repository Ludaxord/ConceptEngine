#pragma once
#include "CEDelegate.h"
#include "CEDelegateBase.h"
#include "../Containers/CEArray.h"

namespace ConceptEngine::Core::Delegates {
	class CEDelegateHandle {
		template <typename... TArgs>
		friend class CEMulticastBase;

	public:
		CEDelegateHandle() : Handle(nullptr) {

		}

		bool IsValid() const {
			return Handle != nullptr;
		}

		operator bool() const {
			return IsValid();
		}

	protected:
	private:
		CEDelegateHandle(void* InHandle) : Handle(InHandle) {

		}

		void* Handle;
	};

	template <typename... TArgs>
	class CEMulticastBase : public CEDelegateBase<void(TArgs ...)> {
	protected:
		typedef CEDelegateBase<void(TArgs ...)> Base;

		typedef typename Base::FunctionType FunctionType;
		typedef typename Base::IDelegate IDelegate;
		typedef typename Base::FunctionDelegate FunctionDelegate;

		template <typename T>
		using MemberFunctionType = typename Base::template MemberFunctionType<T>;
		template <typename T>
		using ConstMemberFunctionType = typename Base::template ConstMemberFunctionType<T>;
		template <typename T>
		using ObjectDelegate = typename Base::template ObjectDelegate<T>;
		template <typename T>
		using ConstObjectDelegate = typename Base::template ConstObjectDelegate<T>;
		template <typename T>
		using LambdaDelegate = typename Base::template LambdaDelegate<T>;
	public:
		CEDelegateHandle AddFunction(FunctionType Fn) {
			return InternalAddNewDelegate(new FunctionDelegate(Fn));
		}

		template <typename T>
		CEDelegateHandle AddObject(T* Object, MemberFunctionType<T> Fn) {
			return InternalAddNewDelegate(new ObjectDelegate<T>(Object, Fn));
		}

		template <typename T>
		CEDelegateHandle AddObject(const T* Object, ConstMemberFunctionType<T> Fn) {
			return InternalAddNewDelegate(new ConstMemberFunctionType<T>(Object, Fn));
		}

		template <typename T>
		CEDelegateHandle AddLambda(T Fn) {
			return InternalAddNewDelegate(new LambdaDelegate<T>(Containers::Forward<T>(Fn)));
		}

		CEDelegateHandle AddDelegate(const CEDelegate<void(TArgs ...)>& Delegate) {
			IDelegate* NewDelegate = Delegate.Delegate;
			return InternalAddNewDelegate(NewDelegate->Clone());
		}

		void Unbind(CEDelegateHandle Handle) {
			IDelegate* DelegateHandle = reinterpret_cast<IDelegate*>(Handle.Handle);
			for (typename Containers::CEArray<IDelegate*>::Iterator It = Delegates.Begin(); It != Delegates.End(); ++It) {
				if (DelegateHandle == *It) {
					Delegates.Erase(It);
					return;
				}
			}
		}

		bool IsBound() const {
			return !Delegates.IsEmpty();
		}

		operator bool() const {
			return IsBound();
		}

	protected:
		Containers::CEArray<IDelegate*> Delegates;
	private:
		CEDelegateHandle InternalAddNewDelegate(IDelegate* NewDelegate) {
			Delegates.EmplaceBack(NewDelegate);
			return CEDelegateHandle(NewDelegate);
		}
	};
}
