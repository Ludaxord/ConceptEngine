#pragma once
#include <combaseapi.h>
#include <memory>

#include "Managers/CEGraphicsManager.h"
#include "Managers/CERendererManager.h"
#include "Managers/CETextureManager.h"

#include "Rendering/CEDebugUI.h"
#include "../../Time/CETimestamp.h"
#include "../../Core/Platform/CEPlatformActions.h"


namespace ConceptEngine::Graphics::Main {
	enum class GraphicsAPI {
		Unknown = 0,
		DirectX = 1,
		Vulkan = 2,
		OpenGL = 3,
		Metal = 4,
		Count
	};

	enum class CEShadingRateTier {
		NotSupported = 0,
		Tier1 = 1,
		Tier2 = 2
	};

	struct CEShadingRateSupport {
		CEShadingRateTier Tier = CEShadingRateTier::NotSupported;
		uint32 ShadingRateImageTileSize = 0;
	};

	enum class CERayTracingTier {
		NotSupported = 0,
		Tier1 = 1,
		Tier1_1 = 2
	};

	struct CERayTracingSupport {
		CERayTracingTier Tier;
		uint32 MaxRecursionDepth;
	};

	class CEGraphics {
	public:
		friend class CECore;
		CEGraphics();
		virtual ~CEGraphics() = default;
		virtual bool Create();
		virtual bool CreateManagers();

		virtual void Update(Time::CETimestamp DeltaTime) = 0;
		virtual void Render() = 0;
		virtual void Resize() = 0;
		virtual void Destroy() = 0;

		static Rendering::CEDebugUI* GetDebugUI();

	protected:
		std::unique_ptr<Managers::CEGraphicsManager> m_graphicsManager;
		std::unique_ptr<Managers::CETextureManager> m_textureManager;
		std::unique_ptr<Managers::CERendererManager> m_rendererManager;

		static std::unique_ptr<Rendering::CEDebugUI> m_debugUi;
	private:
		virtual bool CreateGraphicsManager() = 0;
		virtual bool CreateTextureManager() = 0;
		virtual bool CreateRendererManager() = 0;
		virtual bool CreateDebugUi() = 0;
	};
}
