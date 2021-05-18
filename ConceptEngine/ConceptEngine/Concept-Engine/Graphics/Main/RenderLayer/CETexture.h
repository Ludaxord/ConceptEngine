#pragma once
#include "CEResource.h"
#include "CERendering.h"
#include "CEResourceViews.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
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
