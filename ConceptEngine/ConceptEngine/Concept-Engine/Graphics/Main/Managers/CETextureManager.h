#pragma once

#include <string>

#include "../../../Core/Common/CEManager.h"
#include "../../../Core/Common/CETypes.h"
#include "../RenderLayer/CETexture.h"


namespace ConceptEngine::Graphics::Main::Managers {
	enum CETextureFlags : uint32 {
		TextureFlag_None = 0,
		TextureFlag_GenerateMips = FLAG(1)
	};

	class CETextureManager : public Core::Common::CEManager {
	public:
		CETextureManager();
		~CETextureManager() override;
		
		virtual bool Create() override;

		static class RenderLayer::CETexture2D* LoadFromFile(const std::string& filePath, uint32 flags,
		                                                    RenderLayer::CEFormat format);
		static class RenderLayer::CETexture2D* LoadFromMemory(const uint8* pixels, uint32 width, uint32 height,
		                                                      uint32 createFlags, RenderLayer::CEFormat format);

		static class RenderLayer::CETextureCube* CreateTextureCubeFromPanorama(
			class RenderLayer::CETexture2D* panoramaSource, uint32 cubeMapSize, uint32 flags,
			RenderLayer::CEFormat format);
	};
}
