#pragma once

#include "../Platform/Generic/CEPlatform.h"

#define CE_LOG_ERROR(message) \
    { \
        Assert(ConceptEngine::Core::Generic::Platform::CEPlatform::Console != nullptr); \
        ConceptEngine::Core::Generic::Platform::CEPlatform::Console->SetColor(ConceptEngine::Core::Platform::Generic::Window::CEConsoleColor::Red); \
        ConceptEngine::Core::Generic::Platform::CEPlatform::Console->Print(std::string(message) + "\n"); \
        ConceptEngine::Core::Generic::Platform::CEPlatform::Console->SetColor(ConceptEngine::Core::Platform::Generic::Window::CEConsoleColor::White); \
    }

#define CE_LOG_WARNING(message) \
    { \
        Assert(ConceptEngine::Core::Generic::Platform::CEPlatform::Console != nullptr); \
        ConceptEngine::Core::Generic::Platform::CEPlatform::Console->SetColor(ConceptEngine::Core::Platform::Generic::Window::CEConsoleColor::Yellow); \
        ConceptEngine::Core::Generic::Platform::CEPlatform::Console->Print(std::string(message) + "\n"); \
        ConceptEngine::Core::Generic::Platform::CEPlatform::Console->SetColor(ConceptEngine::Core::Platform::Generic::Window::CEConsoleColor::White); \
    }

#define CE_LOG_INFO(message) \
    {\
        Assert(ConceptEngine::Core::Generic::Platform::CEPlatform::Console != nullptr); \
        ConceptEngine::Core::Generic::Platform::CEPlatform::Console->SetColor(ConceptEngine::Core::Platform::Generic::Window::CEConsoleColor::Green); \
        ConceptEngine::Core::Generic::Platform::CEPlatform::Console->Print(std::string(message) + "\n"); \
        ConceptEngine::Core::Generic::Platform::CEPlatform::Console->SetColor(ConceptEngine::Core::Platform::Generic::Window::CEConsoleColor::White); \
    }
