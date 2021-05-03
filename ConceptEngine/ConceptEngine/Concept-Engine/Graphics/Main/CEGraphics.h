#pragma once
#include <combaseapi.h>
#include <memory>

#include "Managers/CEGraphicsManager.h"

namespace ConceptEngine::Graphics::Main {
	enum class GraphicsAPI {
		DirectX,
		Vulkan,
		OpenGL,
		Metal,
		Count
	};

	class CEGraphics {
	public:
		friend class CECore;
		CEGraphics();
		virtual ~CEGraphics() = default;
		virtual void Create() = 0;
		virtual void CreateManagers() = 0;

		virtual void Update() = 0;
		virtual void Render() = 0;
		virtual void Resize() = 0;
		virtual void Destroy() = 0;

		virtual void CreateGraphicsManager() = 0;
	protected:
		std::unique_ptr<Managers::CEGraphicsManager> m_graphicsManager;
	private:
	};
}
