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
	public:
		CEDXBaseTexture2D(CEDXDevice* device, CEFormat format, uint32 sizeX, uint32 sizeY, uint32 sizeZ, uint32 numMips,
		                  uint32 numSamplers, uint32 flags, const CEClearValue& optimalClearValue): CETexture2D(
				format, sizeX, sizeY, numMips, numSamplers, flags, optimalClearValue), CEDXBaseTexture(device),
			RenderTargetView(nullptr), DepthStencilView(nullptr), UnorderedAccessView(nullptr) {

		}

		virtual CERenderTargetView* GetRenderTargetView() const override {
			return RenderTargetView.Get();
		}

		virtual CEDepthStencilView* GetDepthStencilView() const override {
			return DepthStencilView.Get();
		}

		virtual CEUnorderedAccessView* GetUnorderedAccessView() const override {
			return UnorderedAccessView.Get();
		}

		void SetRenderTargetView(CEDXRenderTargetView* renderTargetView) {
			RenderTargetView = renderTargetView;
		}

		void SetDepthStencilView(CEDXDepthStencilView* depthStencilView) {
			DepthStencilView = depthStencilView;
		}

		void SetUnorderedAccessView(CEDXUnorderedAccessView* unorderedAccessView) {
			UnorderedAccessView = unorderedAccessView;
		}

	private:
		Core::Common::CERef<CEDXRenderTargetView> RenderTargetView;
		Core::Common::CERef<CEDXDepthStencilView> DepthStencilView;
		Core::Common::CERef<CEDXUnorderedAccessView> UnorderedAccessView;;
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
