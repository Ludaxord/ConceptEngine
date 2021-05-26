#pragma once
#include "CEDXDeviceElement.h"

#include "../../Main/RenderLayer/CETexture.h"

constexpr uint32 TEXTURE_CUBE_FACE_COUNT = 6;

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXBaseTexture : public CEDXDeviceElement {
	public:
		CEDXBaseTexture(CEDXDevice* device) : CEDXDeviceElement(device), Resource(nullptr) {

		}

		void SetResource(CEDXResource* resource) {
			Resource = resource;
		}

		void SetShaderResourceView(CEDXShaderResourceView* shaderResourceView) {
			ShaderResourceView = shaderResourceView;
		}

		DXGI_FORMAT GetNativeFormat() const {
			return Resource->GetDesc().Format;
		}

		CEDXResource* GetResource() {
			return Resource.Get();
		}

	protected:
		Core::Common::CERef<CEDXResource> Resource;
		Core::Common::CERef<CEDXShaderResourceView> ShaderResourceView;
	};

	class CEDXBaseTexture2D : public Main::RenderLayer::CETexture2D, public CEDXBaseTexture {

	};

	class CEDXBaseTexture2DArray : public Main::RenderLayer::CETexture2DArray, public CEDXBaseTexture {

	};

	class CEDXBaseTextureCube : public Main::RenderLayer::CETextureCube, public CEDXBaseTexture {

	};

	class CEDXBaseTextureCubeArray : public Main::RenderLayer::CETextureCubeArray, public CEDXBaseTexture {

	};

	class CEDXBaseTexture3D : public Main::RenderLayer::CETexture3D, public CEDXBaseTexture {

	};

	template <typename TCEBaseTexture>
	class TCEDXBaseTexture : public TCEBaseTexture {

	};

	using CEDXTexture2D = TCEDXBaseTexture<CEDXBaseTexture2D>;
	using CEDXTexture2DArray = TCEDXBaseTexture<CEDXBaseTexture2DArray>;
	using CEDXTextureCube = TCEDXBaseTexture<CEDXBaseTextureCube>;
	using CEDXTextureCubeArray = TCEDXBaseTexture<CEDXBaseTextureCubeArray>;
	using CEDXTexture3D = TCEDXBaseTexture<CEDXBaseTexture3D>;

	inline CEDXBaseTexture* TextureCast(CETexture* Texture) {
		return nullptr;
	}
	
}
