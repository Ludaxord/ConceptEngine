#pragma once
#include "../../../CEDefinitions.h"

enum class EConsoleColor : uint8
{
    Red    = 0,
    Green  = 1,
    Yellow = 2,
    White  = 3,
    Magenta = 4,
    Cyan = 5,
    Blue = 6,
};

class GenericOutputConsole
{
public:
    virtual ~GenericOutputConsole() = default;

    virtual void Print(const std::string& Message) = 0;
    
    virtual void Clear() = 0;

    virtual void SetTitle(const std::string& Title) = 0;
    virtual void SetColor(EConsoleColor Color)      = 0;

    static GenericOutputConsole* Create();
};

extern GenericOutputConsole* GConsoleOutput;