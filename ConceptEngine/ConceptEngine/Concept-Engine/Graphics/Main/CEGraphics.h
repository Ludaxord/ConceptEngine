#pragma once
#include <combaseapi.h>
#include <memory>

#include "Managers/CEGraphicsManager.h"
#include "Managers/CETextureManager.h"

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
		virtual bool Create();
		virtual bool CreateManagers() ;

		virtual void Update() = 0;
		virtual void Render() = 0;
		virtual void Resize() = 0;
		virtual void Destroy() = 0;

		virtual bool CreateGraphicsManager() = 0;
		virtual bool CreateTextureManager() = 0;

	protected:
		std::unique_ptr<Managers::CEGraphicsManager> m_graphicsManager;
		std::unique_ptr<Managers::CETextureManager> m_textureManager;
	private:
	};
}
