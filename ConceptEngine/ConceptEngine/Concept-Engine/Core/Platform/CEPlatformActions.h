#pragma once

#include "Generic/Utilities/OSUtilities.h"

#if defined(WINDOWS_PLATFORM)
#include "Windows/Actions/CEWindowsActions.h"
typedef ConceptEngine::Core::Platform::Windows::Actions::CEWindowsActions CEPlatformActions;
#elif defined(MACOS_PLATFORM)
#elif defined(IOS_PLATFORM)
#elif defined(LINUX_PLATFORM)
#elif defined(ANDROID_PLATFORM)
#endif
