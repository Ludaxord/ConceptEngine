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
		CERef<CEDXResource> Resource;
		CERef<CEDXShaderResourceView> ShaderResourceView;
	};

	class CEDXBaseTexture2D : public Main::RenderLayer::CETexture2D, public CEDXBaseTexture {
	public:
		CEDXBaseTexture2D(CEDXDevice* device,
		                  CEFormat format,
		                  uint32 sizeX,
		                  uint32 sizeY,
		                  uint32 sizeZ,
		                  uint32 numMips,
		                  uint32 numSamplers,
		                  uint32 flags,
		                  const CEClearValue& optimalClearValue): CETexture2D(
			                                                          format, sizeX, sizeY, numMips, numSamplers, flags,
			                                                          optimalClearValue), CEDXBaseTexture(device),
		                                                          RenderTargetView(nullptr), DepthStencilView(nullptr),
		                                                          UnorderedAccessView(nullptr) {

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
		CERef<CEDXRenderTargetView> RenderTargetView;
		CERef<CEDXDepthStencilView> DepthStencilView;
		CERef<CEDXUnorderedAccessView> UnorderedAccessView;;
	};

	class CEDXBaseTexture2DArray : public Main::RenderLayer::CETexture2DArray, public CEDXBaseTexture {
	public:
		CEDXBaseTexture2DArray(CEDXDevice* device, CEFormat format, uint32 sizeX, uint32 sizeY, uint32 sizeZ,
		                       uint32 numMips, uint32 numSamplers, uint32 flags, const CEClearValue& clearValue) :
			CETexture2DArray(format, sizeX, sizeY, numMips, numSamplers, sizeZ, flags, clearValue),
			CEDXBaseTexture(device) {

		}
	};

	class CEDXBaseTextureCube : public Main::RenderLayer::CETextureCube, public CEDXBaseTexture {
	public:
		CEDXBaseTextureCube(CEDXDevice* device, CEFormat format, uint32 sizeX, uint32 sizeY, uint32 sizeZ,
		                    uint32 numMips, uint32 numSamplers, uint32 flags,
		                    const CEClearValue& clearValue) : CETextureCube(format, sizeX, numMips, flags, clearValue),
		                                                      CEDXBaseTexture(device) {

		}
	};

	class CEDXBaseTextureCubeArray : public Main::RenderLayer::CETextureCubeArray, public CEDXBaseTexture {
	public:
		CEDXBaseTextureCubeArray(CEDXDevice* device, CEFormat format, uint32 sizeX, uint32 sizeY, uint32 sizeZ,
		                         uint32 numMips, uint32 numSamplers, uint32 flags,
		                         const CEClearValue& clearValue) : CETextureCubeArray(format, sizeX, numMips, sizeZ,
			                                                           flags,
			                                                           clearValue), CEDXBaseTexture(device) {

		}
	};

	class CEDXBaseTexture3D : public Main::RenderLayer::CETexture3D, public CEDXBaseTexture {
	public:
		CEDXBaseTexture3D(CEDXDevice* device, CEFormat format, uint32 sizeX, uint32 sizeY, uint32 sizeZ,
		                  uint32 numMips, uint32 numSamplers, uint32 flags,
		                  const CEClearValue& clearValue): CETexture3D(format, sizeX, sizeY, sizeZ, numMips, flags,
		                                                               clearValue), CEDXBaseTexture(device) {

		}
	};

	template <typename TCEBaseTexture>
	class TCEDXBaseTexture : public TCEBaseTexture {
	public:
		TCEDXBaseTexture(CEDXDevice* device, CEFormat format, uint32 sizeX, uint32 sizeY, uint32 sizeZ,
		                 uint32 numMips, uint32 numSamplers, uint32 flags,
		                 const CEClearValue& clearValue) : TCEBaseTexture(
			device, format, sizeX, sizeY, sizeZ, numMips, numSamplers, flags, clearValue) {

		}
	};

	using CEDXTexture2D = TCEDXBaseTexture<CEDXBaseTexture2D>;
	using CEDXTexture2DArray = TCEDXBaseTexture<CEDXBaseTexture2DArray>;
	using CEDXTextureCube = TCEDXBaseTexture<CEDXBaseTextureCube>;
	using CEDXTextureCubeArray = TCEDXBaseTexture<CEDXBaseTextureCubeArray>;
	using CEDXTexture3D = TCEDXBaseTexture<CEDXBaseTexture3D>;

	inline CEDXBaseTexture* TextureCast(CETexture* Texture) {
		Assert(Texture != nullptr);

		if (Texture->AsTexture2D() != nullptr) {
			return static_cast<CEDXTexture2D*>(Texture);
		}
		else if (Texture->AsTexture2DArray() != nullptr) {
			return static_cast<CEDXTexture2DArray*>(Texture);
		}
		else if (Texture->AsTextureCube() != nullptr) {
			return static_cast<CEDXTextureCube*>(Texture);
		}
		else if (Texture->AsTextureCubeArray() != nullptr) {
			return static_cast<CEDXTextureCubeArray*>(Texture);
		}
		else if (Texture->AsTexture3D() != nullptr) {
			return static_cast<CEDXTexture3D*>(Texture);
		}

		return nullptr;
	}

}
