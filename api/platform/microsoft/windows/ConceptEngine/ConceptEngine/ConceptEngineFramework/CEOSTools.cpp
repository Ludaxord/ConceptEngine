#include "CEOSTools.h"

#include <GL/glew.h>


#include "CETools.h"

CEOSTools::CEOSType CEOSTools::GetOperatingSystem() {
#ifdef _WIN64
	return CEOSType::Windows_64Bit;
#elif _WIN32
	return CEOSType::Windows_32_64Bit;
#elif __APPLE__
    return CEOSType::MacOS;
#elif __linux__
    return CEOSType::Linux;
#elif TARGET_OS_EMBEDDED
    return CEOSType::iOS;
#elif TARGET_IPHONE_SIMULATOR
    return CEOSType::iOS;
#elif TARGET_OS_IPHONE
    return CEOSType::iOS;
#elif TARGET_OS_MAC
    return CEOSType::MacOS;
#elif__ANDROID__
	return CEOSType::Android;
#elif __unix__
    return CEOSType::Unix;
#elif _POSIX_VERSION
    return CEOSType::Postix;
#elif __sun
    return CEOSType::SolarisOS;
#elif __hpux
    return CEOSType::HPUXOS;
#elif BSD
    return CEOSType::BSD;
#elif __DragonFly__
    return CEOSType::DragonFlyBSD;
#elif __FreeBSD__
    return CEOSType::FreeBSD;
#elif __NetBSD__
    return CEOSType::NetBSD;
#elif __OpenBSD__
    return CEOSType::OpenBSD;
#else
    return CEOSType::Undefined;
#endif
}

std::list<CEOSTools::CEGraphicsApiTypes> CEOSTools::GetCompatibleGraphics() {
	std::list<CEGraphicsApiTypes> graphicsTypes;
	//TODO: Check compatible graphics
	return graphicsTypes;
}

bool CEOSTools::CheckVulkanCompatible() {
	return false;
}

const char* CEOSTools::GetVulkanVersion() {
	return "";
}

bool CEOSTools::CheckDirect3DCompatible() {
	return false;
}

const char* CEOSTools::GetDirect3DVersion() {
	return "";
}

bool CEOSTools::CheckMetalCompatible() {
	return false;
}

const char* CEOSTools::GetMetalVersion() {
	return "";
}

bool CEOSTools::CheckOpenGLCompatible() {
	return false;
}

const char* CEOSTools::GetOpenGLVersion() {
	return (char*)glGetString(GL_VERSION);
}
