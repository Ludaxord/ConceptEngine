#include "CEOS.h"
#include <stdio.h>

CEOS::CEOSType CEOS::GetOperatingSystem() {
	printf("Welcome to Concept Engine on");
	// Checking for windows OS with 
	// _WIN32 macro 
#ifdef _WIN64
	return CEOSType::Windows_64Bit;
    // Checking for windows OS with 
	// _WIN32 macro 
#elif _WIN32
	return CEOSType::Windows_32_64Bit;
	// Checking for mac OS with 
	// __APPLE__ macro 
#elif __APPLE__
    return CEOSType::MacOS;
        // Checking for linux OS with 
	// __linux__ macro 
#elif __linux__
    return CEOSType::Linux;
        // Checking for iOS embedded OS with 
	// TARGET_OS_EMBEDDED macro 
#elif TARGET_OS_EMBEDDED
    return CEOSType::iOS;
        // Checking for iOS simulator OS with 
	// TARGET_IPHONE_SIMULATOR macro 
#elif TARGET_IPHONE_SIMULATOR
    return CEOSType::iOS;
        // Checking for iPhone OS with 
	// TARGET_OS_IPHONE macro 
#elif TARGET_OS_IPHONE
    return CEOSType::iOS;
        // Checking for MAC OS with 
	// TARGET_OS_MAC macro 
#elif TARGET_OS_MAC
    return CEOSType::MacOS;
        // Checking for Android OS with 
        // __ANDROID__ macro 
#elif__ANDROID__
	return CEOSType::Android;
        // Checking for unix OS with 
	// __unix__ macro 
#elif __unix__
    return CEOSType::Unix;
        // Checking for POSIX based OS with 
	// _POSIX_VERSION macro 
#elif _POSIX_VERSION
    return CEOSType::Postix;
        // Checking for Solaris OS with 
	// __sun macro 
#elif __sun
    return CEOSType::SolarisOS;
        // Checking for HP UX OS with 
	// __hpux macro 
#elif __hpux
    return CEOSType::HPUXOS;
        // Checking for BSD OS with 
	// BSD macro 
#elif BSD
    return CEOSType::BSD;
        // Checking for DragonFly BSD OS with 
	// __DragonFly__ macro 
#elif __DragonFly__
    return CEOSType::DragonFlyBSD;
        // Checking for FreeBSD OS with 
	// __FreeBSD__ macro 
#elif __FreeBSD__
    return CEOSType::FreeBSD;
        // Checking for Net BSD OS with 
	// __NetBSD__ macro 
#elif __NetBSD__
    return CEOSType::NetBSD;
        // Checking for Open BSD OS with 
	// __OpenBSD__ macro 
#elif __OpenBSD__
    return CEOSType::OpenBSD;
        // If neither of them is present 
	// then this is printed... 
#else
    return CEOSType::Undefined;
#endif
}

std::list<CEGraphics::CEGraphicsApiTypes> CEOS::GetCompatibleGraphics() {
	std::list<CEGraphics::CEGraphicsApiTypes> graphicsTypes;
	//TODO: Check compatible graphics
	return graphicsTypes;
}

bool CEOS::CheckVulkanCompatible() {
	return false;
}

const char* CEOS::CheckVulkanVersion() {
	return "";
}

bool CEOS::CheckDirect3DCompatible() {
	return false;
}

const char* CEOS::CheckDirect3DVersion() {
	return "";
}

bool CEOS::CheckMetalCompatible() {
	return false;
}

const char* CEOS::CheckMetalVersion() {
	return "";
}

bool CEOS::CheckOpenGLCompatible() {
	return false;
}

const char* CEOS::CheckOpenGLVersion() {
	return "";
}
