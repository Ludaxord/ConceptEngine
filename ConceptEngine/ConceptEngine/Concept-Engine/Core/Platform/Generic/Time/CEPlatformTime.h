#pragma once

#include "../Utilities/OSUtilities.h"

#if defined(WINDOWS_PLATFORM)
#include "../../Windows/Time/CEWindowsTime.h"
typedef ConceptEngine::Core::Platform::Windows::Time::CEWindowsTime CEPlatformTime;
#elif defined(MACOS_PLATFORM)
#elif defined(IOS_PLATFORM)
#elif defined(LINUX_PLATFORM)
#elif defined(ANDROID_PLATFORM)
#endif
