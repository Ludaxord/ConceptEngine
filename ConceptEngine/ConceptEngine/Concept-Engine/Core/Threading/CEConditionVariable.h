#pragma once

#include "../Platform/Generic/Utilities/OSUtilities.h"

#if defined(WINDOWS_PLATFORM)
#include "../Platform/Windows/Threading/CEWindowsConditionVariable.h"
typedef ConceptEngine::Core::Platform::Windows::Threading::CEWindowsConditionVariable CEConditionVariable;
#elif defined(MACOS_PLATFORM)
#elif defined(IOS_PLATFORM)
#elif defined(LINUX_PLATFORM)
#elif defined(ANDROID_PLATFORM)
#endif
