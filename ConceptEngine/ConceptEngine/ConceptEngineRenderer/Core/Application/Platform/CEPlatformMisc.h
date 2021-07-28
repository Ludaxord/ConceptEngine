#pragma once
#ifdef PLATFORM_WINDOWS
    #include "../../../Core/Application/Windows/CEWindowsMisc.h"
    typedef CEWindowsMisc CEPlatformMisc;
#else
    #include "../../../Core/Application/Generic/Misc.h"
    typedef Misc CEPlatformMisc;
#endif