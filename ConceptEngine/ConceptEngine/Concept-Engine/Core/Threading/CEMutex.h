#pragma once
#include "../Platform/Generic/Utilities/OSUtilities.h"

#if defined(WINDOWS_PLATFORM)
#include "../Platform/Windows/Threading/CEWindowsMutex.h"
typedef ConceptEngine::Core::Platform::Windows::Threading::CEWindowsMutex CEMutex;
#elif defined(MACOS_PLATFORM)
#elif defined(IOS_PLATFORM)
#elif defined(LINUX_PLATFORM)
#elif defined(ANDROID_PLATFORM)
#endif
