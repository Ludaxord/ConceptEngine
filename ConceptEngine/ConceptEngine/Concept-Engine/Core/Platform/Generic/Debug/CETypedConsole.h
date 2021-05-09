#pragma once
#include "../Utilities/OSUtilities.h"

#if defined(WINDOWS_PLATFORM)
#include "../../Windows/Debug/CEWindowsCommandConsole.h"
typedef ConceptEngine::Core::Platform::Windows::Debug::CEWindowsCommandConsole CETypedConsole;
#elif defined(MACOS_PLATFORM)
#include "../../Mac/Debug/CEMacCommandConsole.h"
typedef ConceptEngine::Core::Platform::Windows::Debug::CEMacCommandConsole CETypedConsole;
#elif defined(IOS_PLATFORM)
#include "../../iOS/Debug/CEiOSCommandConsole.h"
typedef ConceptEngine::Core::Platform::Windows::Debug::CEiOSCommandConsole CETypedConsole;
#elif defined(LINUX_PLATFORM)
#include "../../Linux/Debug/CELinuxCommandConsole.h"
typedef ConceptEngine::Core::Platform::Windows::Debug::CELinuxCommandConsole CETypedConsole;
#elif defined(ANDROID_PLATFORM)
#include "../../Android/Debug/CEAndroidCommandConsole.h"
typedef ConceptEngine::Core::Platform::Windows::Debug::CEAndroidCommandConsole CETypedConsole;
#endif

#define INIT_CONSOLE_VARIABLE(Name, Variable) GTypedConsole.RegisterVariable(Name, Variable);
#define INIT_CONSOLE_COMMAND(Name, Command) GTypedConsole.RegisterCommand(Name, Command);

extern CETypedConsole GTypedConsole;
