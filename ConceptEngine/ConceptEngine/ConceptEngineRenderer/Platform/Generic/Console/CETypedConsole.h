#pragma once
//TODO: Implement
// #include "../Utilities/OSUtilities.h"

#if PLATFORM_WINDOWS
#include "Platform/Windows/Console/CEWindowsCommandConsole.h"
typedef CEWindowsCommandConsole CETypedConsole;
// #elif defined(MACOS_PLATFORM)
// //TODO: Implement
// #include "../../Mac/Debug/CEMacCommandConsole.h"
// typedef ConceptEngine::Core::Platform::Windows::Debug::CEMacCommandConsole CETypedConsole;
// #elif defined(IOS_PLATFORM)
// //TODO: Implement
// #include "../../iOS/Debug/CEiOSCommandConsole.h"
// typedef ConceptEngine::Core::Platform::Windows::Debug::CEiOSCommandConsole CETypedConsole;
// #elif defined(LINUX_PLATFORM)
// //TODO: Implement
// #include "../../Linux/Debug/CELinuxCommandConsole.h"
// typedef ConceptEngine::Core::Platform::Windows::Debug::CELinuxCommandConsole CETypedConsole;
// #elif defined(ANDROID_PLATFORM)
// //TODO: Implement
// #include "../../Android/Debug/CEAndroidCommandConsole.h"
// typedef ConceptEngine::Core::Platform::Windows::Debug::CEAndroidCommandConsole CETypedConsole;
#endif

extern CETypedConsole GTypedConsole;

#define INIT_CONSOLE_VARIABLE(Name, Variable) GTypedConsole.RegisterVariable(Name, Variable);
#define INIT_CONSOLE_COMMAND(Name, Command) GTypedConsole.RegisterCommand(Name, Command);
