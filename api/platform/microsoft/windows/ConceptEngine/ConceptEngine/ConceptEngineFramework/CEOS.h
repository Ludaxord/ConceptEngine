#pragma once
#include <list>

#include "CEGraphics.h"

class CEOS {
public:
	enum class CEOSType {
		Windows_64Bit,
		Windows_32Bit,
		Windows_32_64Bit,
		Linux,
		Unix,
		MacOS,
		iOS,
		iPadOS,
		Android,
		Undefined
	};

public:
	static CEOSType GetOperatingSystem();
	static std::list<CEGraphics::CEGraphicsApiTypes> GetCompatibleGraphics();

private:
	static bool CheckVulkanCompatible();
	static const char* CheckVulkanVersion();
	static bool CheckDirect3DCompatible();
	static const char* CheckDirect3DVersion();
	static bool CheckMetalCompatible();
	static const char* CheckMetalVersion();
	static bool CheckOpenGLCompatible();
	static const char* CheckOpenGLVersion();
};
