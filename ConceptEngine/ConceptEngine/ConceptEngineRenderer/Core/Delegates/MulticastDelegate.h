#pragma once
#include "MulticastBase.h"

template<typename... TArgs>
class CEMulticastDelegateBase : public CEMulticastBase<TArgs...>
{
protected:
    typedef CEMulticastBase<TArgs...> Base;

    typedef typename Base::IDelegate IDelegate;

public:
    CEMulticastDelegateBase()
        : Base()
    {
    }

    CEMulticastDelegateBase(const CEMulticastDelegateBase& Other)
        : Base()
    {
        for (IDelegate* Delegate : Other.Delegates)
        {
            Assert(Delegate != nullptr);
            Base::Delegates.EmplaceBack(Delegate->Clone());
        }
    }

    CEMulticastDelegateBase(CEMulticastDelegateBase&& Other)
        : Base()
    {
        Base::Delegates = Move(Other.Delegates);
    }

    ~CEMulticastDelegateBase()
    {
        UnbindAll();
    }

    void UnbindAll()
    {
        for (IDelegate* Delegate : Base::Delegates)
        {
            Assert(Delegate != nullptr);
            delete Delegate;
        }

        Base::Delegates.Clear();
    }

    void Swap(CEMulticastDelegateBase& Other)
    {
        CEMulticastDelegateBase Temp(Move(*this));
        Base::Delegates = Move(Other.Delegates);
        Other.Delegates  = Move(Temp.Delegates);
    }

    CEMulticastDelegateBase& operator=(const CEMulticastDelegateBase& RHS)
    {
        CEMulticastDelegateBase(RHS).Swap(*this);
        return *this;
    }

    CEMulticastDelegateBase& operator=(CEMulticastDelegateBase&& RHS)
    {
        CEMulticastDelegateBase(Move(RHS)).Swap(*this);
        return *this;
    }
};

template<typename... TArgs>
class TMulticastDelegate : public CEMulticastDelegateBase<TArgs...>
{
    typedef CEMulticastDelegateBase<TArgs...> Base;

    typedef typename Base::IDelegate IDelegate;

public:
    void Broadcast(TArgs... Args)
    {
        for (IDelegate* Delegate : Base::Delegates)
        {
            Assert(Delegate != nullptr);
            Delegate->Execute(Forward<TArgs>(Args)...);
        }
    }

    void operator()(TArgs... Args)
    {
        return Broadcast(Forward<TArgs>(Args)...);
    }
};

template<>
class TMulticastDelegate<void> : public CEMulticastDelegateBase<void>
{
    typedef CEMulticastDelegateBase<void> Base;

    typedef typename Base::IDelegate IDelegate;

public:
    void Broadcast()
    {
        for (IDelegate* Delegate : Base::Delegates)
        {
            Assert(Delegate != nullptr);
            Delegate->Execute();
        }
    }

    void operator()()
    {
        return Broadcast();
    }
};