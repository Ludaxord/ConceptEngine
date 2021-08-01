#pragma once
#ifdef PLATFORM_WINDOWS
    #include "../../../Core/Application/Windows/CEWindowsPlatform.h"
    typedef CEWindowsPlatform Platform;
#else
    #include "../../../Core/Application/Generic/GenericPlatform.h"
    typedef GenericPlatform Platform;
#endif