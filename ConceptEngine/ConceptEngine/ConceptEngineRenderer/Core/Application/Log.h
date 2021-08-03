#pragma once
#include "Generic/CEConsole.h"
#include "../../Platform/CEPlatform.h"


#define CE_LOG_ERROR(Message) \
    { \
        Assert(CEPlatform::Console != nullptr); \
        CEPlatform::Console->SetColor(EConsoleColor::Red); \
        CEPlatform::Console->Print(std::string(Message) + "\n"); \
        CEPlatform::Console->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_WARNING(Message) \
    { \
        Assert(CEPlatform::Console != nullptr); \
        CEPlatform::Console->SetColor(EConsoleColor::Yellow); \
        CEPlatform::Console->Print(std::string(Message) + "\n"); \
        CEPlatform::Console->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_INFO(Message) \
    {\
        Assert(CEPlatform::Console != nullptr); \
        CEPlatform::Console->SetColor(EConsoleColor::Green); \
        CEPlatform::Console->Print(std::string(Message) + "\n"); \
        CEPlatform::Console->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_VERBOSE(Message) \
    {\
        Assert(CEPlatform::Console != nullptr); \
        CEPlatform::Console->SetColor(EConsoleColor::Blue); \
        CEPlatform::Console->Print(std::string(Message) + "\n"); \
        CEPlatform::Console->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_DEBUG(Message) \
    {\
        Assert(CEPlatform::Console != nullptr); \
        CEPlatform::Console->SetColor(EConsoleColor::Magenta); \
        CEPlatform::Console->Print(std::string(Message) + "\n"); \
        CEPlatform::Console->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_MESSAGE(Message) \
    {\
        Assert(CEPlatform::Console != nullptr); \
        CEPlatform::Console->SetColor(EConsoleColor::Cyan); \
        CEPlatform::Console->Print(std::string(Message) + "\n"); \
        CEPlatform::Console->SetColor(EConsoleColor::White); \
    }
