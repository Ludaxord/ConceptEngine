#pragma once
#include <stdio.h>

/**
 * Soruce: https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive
 * Determination a platform of an operation system
 * Fully supported supported only GNU GCC/G++, partially on Clang/LLVM
 */

#if defined(_WIN32)
	#define PLATFORM_NAME "windows" // Windows
	#define WINDOWS_PLATFORM
#elif defined(_WIN64)
    #define PLATFORM_NAME "windows" // Windows
	#define WINDOWS_PLATFORM
#elif defined(__CYGWIN__) && !defined(_WIN32)
    #define PLATFORM_NAME "windows" // Windows (Cygwin POSIX under Microsoft Window)
	#define WINDOWS_PLATFORM
#elif defined(__ANDROID__)
    #define PLATFORM_NAME "android" // Android (implies Linux, so it must come first)
	#define ANDROID_PLATFORM
#elif defined(__linux__)
    #define PLATFORM_NAME "linux" // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other
	#define LINUX_PLATFORM
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
    #include <sys/param.h>
#if defined(BSD)
        #define PLATFORM_NAME "bsd" // FreeBSD, NetBSD, OpenBSD, DragonFly BSD
		#define BSD_PLATFORM
#endif
#elif defined(__hpux)
    #define PLATFORM_NAME "hp-ux" // HP-UX
	#define HP_UX_PLATFORM
#elif defined(_AIX)
    #define PLATFORM_NAME "aix" // IBM AIX
	#define AIX_PLATFORM
#elif defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
    #include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
        #define PLATFORM_NAME "ios" // Apple iOS
		#define IOS_PLATFORM
#elif TARGET_OS_IPHONE == 1
        #define PLATFORM_NAME "ios" // Apple iOS
		#define IOS_PLATFORM
#elif TARGET_OS_MAC == 1
        #define PLATFORM_NAME "macos" // Apple OSX
		#define MACOS_PLATFORM
#endif
#elif defined(__sun) && defined(__SVR4)
    #define PLATFORM_NAME "solaris" // Oracle Solaris, Open Indiana
	#define SOLARIS_PLATFORM
#else
    #define PLATFORM_NAME NULL
	#define UNKNOWN_PLATFORM
#endif

// Return a name of platform, if determined, otherwise - an empty string
inline const char* GetPlatformName() {
	return (PLATFORM_NAME == NULL) ? "" : PLATFORM_NAME;
}

#define PLATFORM() GetPlatformName()
