#pragma once
#include <combaseapi.h>

namespace ConceptEngine::Graphics::Main {
	enum class GraphicsAPI {
		DirectX,
		Vulkan,
		OpenGL,
		Metal,
		Count
	};

	interface CEGraphics {
	public:
		friend class CECore;
		virtual ~CEGraphics() = default;
		virtual void Create() = 0;
		virtual void CreateManagers() = 0;
	protected:
	private:
	};
}
