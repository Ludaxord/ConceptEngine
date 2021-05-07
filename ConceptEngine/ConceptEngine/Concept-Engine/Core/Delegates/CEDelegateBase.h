#pragma once
#include "../Containers/CEContainerUtilities.h"

namespace ConceptEngine::Core::Delegates {
	template <typename TInvokable>
	class CEDelegateBase;

	template <typename TReturn, typename... TArgs>
	class CEDelegateBase<TReturn(TArgs ...)> {
	protected:
		typedef TReturn (*FunctionType)(TArgs ...);

		template <typename T>
		using MemberFunctionType = TReturn (T::*)(TArgs ...);
		template <typename T>
		using ConstMemberFunctionType = TReturn(T::*)(TArgs ...) const;

		struct IDelegate {
			virtual ~IDelegate() = default;
			virtual TReturn Execute(TArgs ... Args) const = 0;
			virtual IDelegate* Clone() const = 0;
		};

		struct FunctionDelegate : public IDelegate {
			FunctionDelegate(FunctionType InFn): IDelegate(), Fn(InFn) {

			}

			virtual TReturn Execute(TArgs ... Args) const override {
				return Fn(Containers::Forward<TArgs>(Args)...);
			}

			virtual IDelegate* Clone() const override {
				return new FunctionDelegate(Fn);
			}

			FunctionType Fn;
		};

		template <typename T>
		struct ObjectDelegate : public IDelegate {
			ObjectDelegate(T* InObject, MemberFunctionType<T> InFn) : IDelegate(), Object(InObject), Fn(InFn) {

			}

			TReturn Execute(TArgs ... Args) const override {
				return ((*Object).*Fn)(Containers::Forward<TArgs>(Args)...);
			};

			IDelegate* Clone() const override {
				return new ObjectDelegate(Object, Fn);
			};
			T* Object;
			MemberFunctionType<T> Fn;
		};

		template <typename T>
		struct ConstObjectDelegate : public IDelegate {
			ConstObjectDelegate(T* InObject, ConstMemberFunctionType<T> InFn) : IDelegate(), Object(InObject),
				Fn(InFn) {

			}

			TReturn Execute(TArgs ... Args) const override {
				return ((*Object).*Fn)(Containers::Forward<TArgs>(Args)...);
			};

			IDelegate* Clone() const override {
				return new ObjectDelegate(Object, Fn);
			};
			T* Object;
			ConstMemberFunctionType<T> Fn;
		};

		template <typename T>
		struct LambdaDelegate : public IDelegate {

			LambdaDelegate(T InInvokable) : IDelegate(), Invokable(InInvokable) {

			}

			TReturn Execute(TArgs ... Args) const override {
				return Invokable(Containers::Forward<TArgs>(Args)...);
			}

			IDelegate* Clone() const override {
				return new LambdaDelegate(Invokable);
			}

			T Invokable;
		};
	};
}
