#pragma once
#include "CEConsoleObject.h"

#include "../../Core/Delegates/MulticastDelegate.h"

#include <cstdlib>
#include <sstream>

class CEConsoleVariable : public CEConsoleObject
{
public:
    virtual CEConsoleVariable* AsVariable() override { return this; }

    virtual void SetInt(int32 InValue) = 0;
    virtual void SetFloat(float InValue) = 0;
    virtual void SetBool(bool InValue) = 0;
    virtual void SetString(const String& InValue) = 0;

    virtual int32 GetInt() const = 0;
    virtual float GetFloat() const = 0;
    virtual bool GetBool() const = 0;
    virtual String GetString() const = 0;

    virtual bool IsInt() const = 0;
    virtual bool IsFloat() const = 0;
    virtual bool IsBool() const = 0;
    virtual bool IsString() const = 0;
    
    TMulticastDelegate<CEConsoleVariable*> OnChangedDelegate;
};

template<typename T>
class TCEConsoleVariable : public CEConsoleVariable
{
public:
    TCEConsoleVariable()
        : CEConsoleVariable()
        , Value()
    {
    }

    TCEConsoleVariable(T StartValue)
        : CEConsoleVariable()
        , Value(StartValue)
    {
    }

    virtual void SetInt(int32 InValue) override 
    { 
        Value = (T)InValue;
        OnChanged();
    }

    virtual void SetFloat(float InValue) override
    {
        Value = (T)InValue;
        OnChanged();
    }

    virtual void SetBool(bool InValue) override
    {
        Value = (T)InValue;
        OnChanged();
    }

    virtual void SetString(const String& InValue) override;

    virtual int32 GetInt() const override
    {
        return (int32)Value;
    }

    virtual float GetFloat() const override
    {
        return (float)Value;
    }

    virtual bool GetBool() const override
    {
        return (bool)Value;
    }

    virtual String GetString() const override
    {
        return std::to_string(Value);
    }

    virtual bool IsInt() const override { return false; }
    virtual bool IsFloat() const override { return false; }
    virtual bool IsBool() const override { return false; }
    virtual bool IsString() const override { return false; }

private:
    void OnChanged()
    {
        if (OnChangedDelegate.IsBound())
        {
            OnChangedDelegate.Broadcast(this);
        }
    }

    T Value;
};

// int32
template<> inline void TCEConsoleVariable<int32>::SetString(const String& InValue)
{
    Value = atoi(InValue.c_str());
    OnChanged();
}

template<> inline bool TCEConsoleVariable<int32>::IsInt() const
{ 
    return true; 
}

// float
template<> inline void TCEConsoleVariable<float>::SetString(const String& InValue)
{
    Value = (float)atof(InValue.c_str());
    OnChanged();
}

template<> inline bool TCEConsoleVariable<float>::IsFloat() const
{
    return true;
}

// bool
template<> inline void TCEConsoleVariable<bool>::SetString(const String& InValue)
{
    String Lower = InValue;
    for (char& c : Lower)
    {
        c = (char)tolower(c);
    }

    int32 Number = 0;
    std::istringstream Stream(Lower);
    Stream >> Number;

    if (Stream.fail())
    {
        Stream.clear();
        Stream >> std::boolalpha >> Value;
    }
    else
    {
        Value = (bool)Number;
    }

    if (!Stream.fail())
    {
        OnChanged();
    }
}

template<> inline String TCEConsoleVariable<bool>::GetString() const
{
    return Value ? "true" : "false";
}

template<> inline bool TCEConsoleVariable<bool>::IsBool() const
{
    return true;
}

// String
template<> inline void TCEConsoleVariable<String>::SetInt(int32 InValue)
{
    Value = std::to_string(InValue);
}

template<> inline void TCEConsoleVariable<String>::SetFloat(float InValue)
{
    Value = std::to_string(InValue);
}

template<> inline void TCEConsoleVariable<String>::SetBool(bool InValue)
{
    std::stringstream Stream;
    Stream << std::boolalpha << InValue;
    Value = Stream.str();
}

template<> inline void TCEConsoleVariable<String>::SetString(const String& InValue)
{
    Value = InValue;
}

template<> inline bool TCEConsoleVariable<String>::IsString() const
{
    return true;
}

template<> inline int32 TCEConsoleVariable<String>::GetInt() const
{
    return 0;
}

template<> inline float TCEConsoleVariable<String>::GetFloat() const
{
    return 0.0f;
}

template<> inline bool TCEConsoleVariable<String>::GetBool() const
{
    return false;
}

template<> inline String TCEConsoleVariable<String>::GetString() const
{
    return Value;
}