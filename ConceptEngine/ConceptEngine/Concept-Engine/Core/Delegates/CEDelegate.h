#pragma once
#include "CEDelegateBase.h"

namespace ConceptEngine::Core::Delegates {
	template <typename TInvokable>
	class CEDelegate;

	template <typename TReturn, typename... TArgs>
	class CEDelegate<TReturn(TArgs ...)> : private CEDelegateBase<TReturn(TArgs ...)> {
		typedef CEDelegateBase<TReturn(TArgs ...)> Base;

		typedef typename Base::IDelegate IDelegate;
		typedef typename Base::FunctionType FunctionType;
		typedef typename Base::FunctionDelegate FunctionDelegate;

		template <typename T>
		using MemberFunctionType = typename Base::template MemberFunctionType<T>;
		template <typename T>
		using ConstMemberFunctionType = typename Base::template ConstMemberFunctionType<T>;
		template <typename T>
		using ObjectDelegate = typename Base::template ObjectDelegate<T>;
		template <typename T>
		using ConstObjectDelegate = typename Base::template ConstObjectDelegate<T>;
		template <typename F>
		using LambdaDelegate = typename Base::template LambdaDelegate<F>;

		template <typename... TArgs>
		friend class TMulticastBase;

	public:
		CEDelegate()
			: Base()
			  , Delegate(nullptr) {
		}

		CEDelegate(const CEDelegate& Other)
			: Base()
			  , Delegate(nullptr) {
			if (Other.IsBound()) {
				Delegate = Other.Delegate->Clone();
			}
		}

		CEDelegate(CEDelegate&& Other)
			: Base()
			  , Delegate(Other.Delegate) {
			Other.Delegate = nullptr;
		}

		~CEDelegate() {
			Unbind();
		}

		void BindFunction(FunctionType Fn) {
			Unbind();
			Delegate = new FunctionDelegate(Fn);
		}

		template <typename T>
		void BindObject(T* This, MemberFunctionType<T> Fn) {
			Unbind();
			Delegate = new ObjectDelegate<T>(This, Fn);
		}

		template <typename T>
		void BindObject(const T* This, ConstMemberFunctionType<T> Fn) {
			Unbind();
			Delegate = new ConstObjectDelegate<T>(This, Fn);
		}

		template <typename F>
		void BindLambda(F Functor) {
			Unbind();
			Delegate = new LambdaDelegate<F>(Containers::Forward<F>(Functor));
		}

		void Unbind() {
			if (Delegate) {
				delete Delegate;
				Delegate = nullptr;
			}
		}

		TReturn Execute(TArgs ... Args) {
			Assert(Delegate != nullptr);
			return Delegate->Execute(Containers::Forward<TArgs>(Args)...);
		}

		bool ExecuteIfBound(TArgs ... Args) {
			if (IsBound()) {
				Delegate->Execute(Containers::Forward<TArgs>(Args)...);
				return true;
			}
			else {
				return false;
			}
		}

		void Swap(CEDelegate& Other) {
			CEDelegate Temp(Containers::Move(*this));
			Delegate = Other.Delegate;
			Other.Delegate = Temp.Delegate;
			Temp.Delegate = nullptr;
		}

		bool IsBound() const {
			return Delegate != nullptr;
		}

		TReturn operator()(TArgs ... Args) {
			return Execute(Containers::Forward<TArgs>(Args)...);
		}

		CEDelegate& operator=(CEDelegate&& RHS) {
			CEDelegate(Containers::Move(RHS)).Swap(*this);
			return *this;
		}

		CEDelegate& operator=(const CEDelegate& RHS) {
			CEDelegate(RHS).Swap(*this);
			return *this;
		}

		operator bool() const {
			return IsBound();
		}

	private:
		IDelegate* Delegate;
	};
}
