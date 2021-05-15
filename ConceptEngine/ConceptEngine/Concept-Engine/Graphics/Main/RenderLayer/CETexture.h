#pragma once
#include "CEResource.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CETexture : public CEResource {
	public:
		explicit CETexture(CEFormat format)
			: Format(format) {
		}

		CEFormat GetFormat() const {
			return Format;
		}

	private:
		CEFormat Format;
	};

	class CETexture2D : public CETexture {

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
