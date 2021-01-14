#include "CEOSTools.h"


#include <activation.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

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

const char* CEOSTools::GetGPUProvider(std::string providerDescription) {
	std::string nvidia = "Nvidia";
	std::string amd = "AMD";
	const char* provider = "";
	if (providerDescription.find(nvidia) != std::string::npos) {
		provider = "             @.8;8;8;8;8;8;8;8;88.      "
			"  .  . .  ..;8:@. @;8%8%8;8%8;8%8: .  . "
			"   .  .%@88888 :88888@X;8;88:8;@8:.     "
			"     S88t;St;888;;   :@:8t8t8;%%8   .  ."
			" ..888%8t@888S8. 88.@ XX;.8.88S88:.   . "
			"  S.8% S@:8S 88% X8@8:%;8t%8;..X8;      "
			"  .%;%888t@%;S88;8888%:88S:8 t888  . .  "
			"   .:%8@S 88X%.8:8%:@88ttS % :t88.     ."
			"     :Stt;;XXS 8 S8 S8XS .t888;88:  .   "
			" . .    8@ ;XXX@S88;@@888.SS:88.8:    . "
			"        ;  S8X 88@8.88. %:8t8t888: .    "
			"   .       .%888@ @8S8:@ SX8%8.XS.   .  "
			"     . .     . .                   .   ."
			";;t;t8t:St%  8t8%S8.8%t%@S  88%  :Xttt  "
			"SS@@SX:8.X:.: 8. S S@%@8%8% St.  t@8St  "
			"S8%X8:t:;8;%S8:% @ SX8X;S@8@ @: %t8:%88 "
			"S88 .8X8. @@ @t .X SX88:8X%X 8:. 8   @:X"
			"8X8  @8X S%X88   8SX;@S@8%;.S8 .@XXX@@8S";
	}

	return provider;
}

CEOSTools::CESystemInfo CEOSTools::GetEngineSystemInfo() {
    CEOSTools::CESystemInfo systemInfo = {};
    int CPUInfo[4] = { -1 };
    unsigned   nExIds, i = 0;
    char CPUBrandString[0x40];
    // Get the information associated with each extended ID.
    __cpuid(CPUInfo, 0x80000000);
    nExIds = CPUInfo[0];
    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
    //string includes manufacturer, model and clockspeed
    systemInfo.CPUName = CPUBrandString;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    systemInfo.CPUCores = sysInfo.dwNumberOfProcessors;

    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    systemInfo.RamSize = (statex.ullTotalPhys / 1024) / 1024;
    return systemInfo;
}
