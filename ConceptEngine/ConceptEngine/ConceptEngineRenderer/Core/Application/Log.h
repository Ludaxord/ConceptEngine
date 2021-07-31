#pragma once
#include "Generic/CEConsole.h"

#define CE_LOG_ERROR(Message) \
    { \
        Assert(GConsoleOutput != nullptr); \
        GConsoleOutput->SetColor(EConsoleColor::Red); \
        GConsoleOutput->Print(std::string(Message) + "\n"); \
        GConsoleOutput->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_WARNING(Message) \
    { \
        Assert(GConsoleOutput != nullptr); \
        GConsoleOutput->SetColor(EConsoleColor::Yellow); \
        GConsoleOutput->Print(std::string(Message) + "\n"); \
        GConsoleOutput->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_INFO(Message) \
    {\
        Assert(GConsoleOutput != nullptr); \
        GConsoleOutput->SetColor(EConsoleColor::Green); \
        GConsoleOutput->Print(std::string(Message) + "\n"); \
        GConsoleOutput->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_VERBOSE(Message) \
    {\
        Assert(GConsoleOutput != nullptr); \
        GConsoleOutput->SetColor(EConsoleColor::Blue); \
        GConsoleOutput->Print(std::string(Message) + "\n"); \
        GConsoleOutput->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_DEBUG(Message) \
    {\
        Assert(GConsoleOutput != nullptr); \
        GConsoleOutput->SetColor(EConsoleColor::Magenta); \
        GConsoleOutput->Print(std::string(Message) + "\n"); \
        GConsoleOutput->SetColor(EConsoleColor::White); \
    }

#define CE_LOG_MESSAGE(Message) \
    {\
        Assert(GConsoleOutput != nullptr); \
        GConsoleOutput->SetColor(EConsoleColor::Cyan); \
        GConsoleOutput->Print(std::string(Message) + "\n"); \
        GConsoleOutput->SetColor(EConsoleColor::White); \
    }