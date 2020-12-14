#pragma once
#pragma once
#include <list>

class CEOSTools {
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

	enum class CEGraphicsApiTypes {
		direct3d11,
		direct3d12,
		vulkan,
		opengl,
		metal
	};

public:
	static CEOSType GetOperatingSystem();
	static std::list<CEGraphicsApiTypes> GetCompatibleGraphics();

public:
	static bool CheckVulkanCompatible();
	static const char* GetVulkanVersion();
	static bool CheckDirect3DCompatible();
	static const char* GetDirect3DVersion();
	static bool CheckMetalCompatible();
	static const char* GetMetalVersion();
	static bool CheckOpenGLCompatible();
	static const char* GetOpenGLVersion();
};
