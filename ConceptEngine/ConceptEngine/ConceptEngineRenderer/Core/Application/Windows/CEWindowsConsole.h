#pragma once
#include "../../../Core/Application/Generic/CEConsole.h"
#include "../../../Core/Threading/Platform/Mutex.h"

#include "../../../Windows/Windows.h"

class CEWindowsConsole : public CEConsole
{
public:
    CEWindowsConsole();
    ~CEWindowsConsole();

    virtual void Print(const std::string& Message) override final;
    
    virtual void Clear() override final;

    virtual void SetTitle(const std::string& Title) override final;
    virtual void SetColor(EConsoleColor Color)      override final;

	virtual void Update() override final;

private:
    HANDLE ConsoleHandle;

    Mutex ConsoleMutex;
};