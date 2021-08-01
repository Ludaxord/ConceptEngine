#pragma once
#include "CEConsoleObject.h"

#include "../../Core/Delegates/MulticastDelegate.h"

class CEConsoleCommand : public CEConsoleObject
{
public:
    virtual CEConsoleCommand* AsCommand() override { return this; }

    void Execute()
    {
        OnExecute.Broadcast();
    }

    TMulticastDelegate<void> OnExecute;
};