#pragma once
#include "../CEHeaderLibs.h"

namespace ConceptEngineFramework::Graphics {
	enum API {
		DirectX12_API,
		Vulkan_API,
		OpenGL_API,
		Metal_API,
		Count
	};

	interface CEGraphicsManager {
	public:
		virtual void Create() = 0;
	protected:
		virtual ~CEGraphicsManager() = default;
	};
}
