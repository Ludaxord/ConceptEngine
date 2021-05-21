#pragma once
#include "CEResource.h"
#include "CERendering.h"
#include "CEResourceViews.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	enum CETextureFlags
{
    TextureFlag_None          = 0,
    TextureFlag_RTV           = FLAG(1), // RenderTargetView
    TextureFlag_DSV           = FLAG(2), // DepthStencilView
    TextureFlag_UAV           = FLAG(3), // UnorderedAccessView
    TextureFlag_SRV           = FLAG(4), // ShaderResourceView
    TextureFlag_NoDefaultRTV  = FLAG(5), // Do not create default RenderTargetView
    TextureFlag_NoDefaultDSV  = FLAG(6), // Do not create default DepthStencilView
    TextureFlag_NoDefaultUAV  = FLAG(7), // Do not create default UnorderedAccessView
    TextureFlag_NoDefaultSRV  = FLAG(8), // Do not create default ShaderResourceView
    TextureFlags_RWTexture    = TextureFlag_UAV | TextureFlag_SRV,
    TextureFlags_RenderTarget = TextureFlag_RTV | TextureFlag_SRV,
    TextureFlags_ShadowMap    = TextureFlag_DSV | TextureFlag_SRV,
};
	
	class CETexture : public CEResource {
	public:
		CEFormat GetFormat() const {
			return Format;
		}

	private:
		CEFormat Format;
	};

	class CETexture2D : public CETexture {
	public:
		virtual class CEShaderResourceView* GetShaderResourceView() const {
			return nullptr;
		};
	};

	class CETexture2DArray : public CETexture2D {

	};

	class CETextureCube : public CETexture {

	};

	class CETextureCubeArray : public CETextureCube {

	};

	class CETexture3D : public CETexture {

	};
}
