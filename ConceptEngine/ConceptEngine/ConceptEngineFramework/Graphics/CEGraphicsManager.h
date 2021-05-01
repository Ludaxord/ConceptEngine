#pragma once
#include "CEPlayground.h"
#include "../CEHeaderLibs.h"
#include "../Game/CETimer.h"

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
		virtual void InitPlayground(CEPlayground* playground) = 0;
		virtual void Destroy() = 0;
		virtual void Resize() = 0;
		virtual void Update(const CETimer& gt) = 0;
		virtual void Render(const CETimer& gt) = 0;
		virtual bool Initialized() = 0;
	protected:
		virtual ~CEGraphicsManager() = default;

	private:
		virtual void CreatePhysics() = 0;
	};
}
