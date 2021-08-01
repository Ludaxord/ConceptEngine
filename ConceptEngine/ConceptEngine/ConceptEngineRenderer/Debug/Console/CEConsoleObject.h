#pragma once
#include "../../CEDefinitions.h"

#include <string>

class CEConsoleVariable;
class CEConsoleCommand;

using String = std::string;

class CEConsoleObject
{
public:
    virtual ~CEConsoleObject() = default;

    virtual CEConsoleCommand*  AsCommand()  { return nullptr; }
    virtual CEConsoleVariable* AsVariable() { return nullptr; }
};