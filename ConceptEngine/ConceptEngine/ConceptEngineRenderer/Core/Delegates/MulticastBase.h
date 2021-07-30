#pragma once
#include "Delegate.h"

#include "../../Core/Containers/Array.h"

class CEDelegateHandle
{
    template<typename... TArgs>
    friend class CEMulticastBase;

public:
    CEDelegateHandle()
        : Handle(nullptr)
    {
    }

    bool IsValid() const
    {
        return Handle != nullptr;
    }

    operator bool() const
    {
        return IsValid();
    }

private:
    CEDelegateHandle(void* InHandle)
        : Handle(InHandle)
    {
    }

    void* Handle;
};

template<typename... TArgs>
class CEMulticastBase : public CEDelegateBase<void(TArgs...)>
{
protected:
    typedef CEDelegateBase<void(TArgs...)> Base;

    typedef typename Base::FunctionType     FunctionType;
    typedef typename Base::IDelegate        IDelegate;
    typedef typename Base::FunctionDelegate FunctionDelegate;

    template<typename T>
    using MemberFunctionType = typename Base::template MemberFunctionType<T>;
    template<typename T>
    using ConstMemberFunctionType = typename Base::template ConstMemberFunctionType<T>;
    template<typename T>
    using ObjectDelegate = typename Base::template ObjectDelegate<T>;
    template<typename T>
    using ConstObjectDelegate = typename Base::template ConstObjectDelegate<T>;
    template<typename F>
    using LambdaDelegate = typename Base::template LambdaDelegate<F>;

public:
    CEDelegateHandle AddFunction(FunctionType Fn)
    {
        return InternalAddNewDelegate(new FunctionDelegate(Fn));
    }

    template<typename T>
    CEDelegateHandle AddObject(T* This, MemberFunctionType<T> Fn)
    {
        return InternalAddNewDelegate(new ObjectDelegate<T>(This, Fn));
    }

    template<typename T>
    CEDelegateHandle AddObject(const T* This, ConstMemberFunctionType<T> Fn)
    {
        return InternalAddNewDelegate(new ConstObjectDelegate<T>(This, Fn));
    }

    template<typename F>
    CEDelegateHandle AddLambda(F Functor)
    {
        return InternalAddNewDelegate(new LambdaDelegate<F>(Forward<F>(Functor)));
    }

    CEDelegateHandle AddDelegate(const TDelegate<void(TArgs...)>& Delegate)
    {
        IDelegate* NewDelegate = Delegate.Delegate;
        return InternalAddNewDelegate(NewDelegate->Clone());
    }

    void Unbind(CEDelegateHandle Handle)
    {
        IDelegate* DelegateHandle = reinterpret_cast<IDelegate*>(Handle.Handle);
        for (typename CEArray<IDelegate*>::Iterator It = Delegates.Begin(); It != Delegates.End(); It++)
        {
            if (DelegateHandle == *It)
            {
                Delegates.Erase(It);
                return;
            }
        }
    }

    bool IsBound() const
    {
        return !Delegates.IsEmpty();
    }

    operator bool() const
    {
        return IsBound();
    }

protected:
    // TODO: Maybe use a list instead?
    CEArray<IDelegate*> Delegates;

private:
    CEDelegateHandle InternalAddNewDelegate(IDelegate* NewDelegate)
    {
        Delegates.EmplaceBack(NewDelegate);
        return CEDelegateHandle(NewDelegate);
    }
};