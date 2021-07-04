#pragma once
#include "CEResource.h"
#include "CERendering.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	enum CETextureFlags {
		TextureFlag_None = 0,
		TextureFlag_RTV = FLAG(1),
		// RenderTargetView
		TextureFlag_DSV = FLAG(2),
		// DepthStencilView
		TextureFlag_UAV = FLAG(3),
		// UnorderedAccessView
		TextureFlag_SRV = FLAG(4),
		// ShaderResourceView
		TextureFlag_NoDefaultRTV = FLAG(5),
		// Do not create default RenderTargetView
		TextureFlag_NoDefaultDSV = FLAG(6),
		// Do not create default DepthStencilView
		TextureFlag_NoDefaultUAV = FLAG(7),
		// Do not create default UnorderedAccessView
		TextureFlag_NoDefaultSRV = FLAG(8),
		// Do not create default ShaderResourceView
		TextureFlags_RWTexture = TextureFlag_UAV | TextureFlag_SRV,
		TextureFlags_RenderTarget = TextureFlag_RTV | TextureFlag_SRV,
		TextureFlags_ShadowMap = TextureFlag_DSV | TextureFlag_SRV,
	};

	class CETexture : public CEResource {
	public:
		CETexture(CEFormat format, uint32 numMips, uint32 flags, const CEClearValue& clearValue) : CEResource(),
			Format(format), NumMips(numMips), Flags(flags), ClearValue(clearValue) {

		}

		virtual class CETexture2D* AsTexture2D() {
			return nullptr;
		}

		virtual class CETexture2DArray* AsTexture2DArray() {
			return nullptr;
		}

		virtual class CETextureCube* AsTextureCube() {
			return nullptr;
		}

		virtual class CETextureCubeArray* AsTextureCubeArray() {
			return nullptr;
		}

		virtual class CETexture3D* AsTexture3D() {
			return nullptr;
		}

		virtual class CEShaderResourceView* GetShaderResourceView() const {
			return nullptr;
		}

		CEFormat GetFormat() const {
			return Format;
		}

		uint32 GetNumMips() const {
			return NumMips;
		}

		uint32 GetFlags() const {
			return Flags;
		}

		const CEClearValue& GetClearValue() const {
			return ClearValue;
		}

		bool IsUAV() const {
			return (Flags & TextureFlag_UAV) && !(Flags & TextureFlag_NoDefaultUAV);
		}

		bool IsSRV() const {
			return (Flags & TextureFlag_SRV) && !(Flags & TextureFlag_NoDefaultSRV);
		}

		bool IsRTV() const {
			return (Flags & TextureFlag_RTV) && !(Flags & TextureFlag_NoDefaultRTV);
		}

		bool IsDSV() const {
			return (Flags & TextureFlag_DSV) && !(Flags & TextureFlag_NoDefaultDSV);
		}

	private:
		CEFormat Format;
		uint32 NumMips;
		uint32 Flags;
		CEClearValue ClearValue;
	};

	class CETexture2D : public CETexture {
	public:
		CETexture2D(CEFormat format, uint32 width, uint32 height, uint32 numMips, uint32 numSamplers, uint32 flags,
		            const CEClearValue& optimizedClearValue): CETexture(format, numMips, flags, optimizedClearValue),
		                                                      Width(width), Height(height), NumSamplers(numSamplers) {

		}

		virtual CETexture2D* AsTexture2D() override {
			return this;
		}

		virtual class CERenderTargetView* GetRenderTargetView() const {
			return nullptr;
		}

		virtual class CEDepthStencilView* GetDepthStencilView() const {
			return nullptr;
		}

		virtual class CEUnorderedAccessView* GetUnorderedAccessView() const {
			return nullptr;
		};

		uint32 GetWidth() const {
			return Width;
		}

		uint32 GetHeight() const {
			return Height;
		}

		uint32 GetNumSamplers() const {
			return NumSamplers;
		}

		bool IsMultiSampled() const {
			return NumSamplers > 1;
		}

	private:
		uint32 Width;
		uint32 Height;
		uint32 NumSamplers;
	};

	class CETexture2DArray : public CETexture2D {
	public:
		CETexture2DArray(CEFormat format, uint32 width, uint32 height, uint32 numMips, uint32 numSamplers,
		                 uint32 numArraySlices, uint32 flags, const CEClearValue& clearValue) :
			CETexture2D(format, width, height, numMips, numSamplers, flags, clearValue),
			NumArraySlices(numArraySlices) {

		}

		virtual CETexture2D* AsTexture2D() override {
			return nullptr;
		}

		virtual CETexture2DArray* AsTexture2DArray() override {
			return this;
		}

		uint32 GetNumArraySlices() const {
			return NumArraySlices;
		}

	private:
		uint32 NumArraySlices;
	};

	class CETextureCube : public CETexture {
	public:
		CETextureCube(CEFormat format, uint32 size, uint32 numMips, uint32 flags,
		              const CEClearValue& clearValue) : CETexture(format, numMips, flags, clearValue), Size(size) {

		}

		virtual CETextureCube* AsTextureCube() override {
			return this;
		}

		uint32 GetSize() const {
			return Size;
		}

	private:
		uint32 Size;
	};

	class CETextureCubeArray : public CETextureCube {
	public:
		CETextureCubeArray(CEFormat format, uint32 size, uint32 numMips, uint32 numArraySlices, uint32 flags,
		                   const CEClearValue& clearValue) : CETextureCube(format, size, numMips, flags, clearValue),
		                                                     NumArraySlices(numArraySlices) {

		}

		virtual CETextureCube* AsTextureCube() override {
			return nullptr;
		}

		virtual CETextureCubeArray* AsTextureCubeArray() override {
			return this;
		}

	private:
		uint32 NumArraySlices;
	};

	class CETexture3D : public CETexture {
	public:
		CETexture3D(CEFormat format, uint32 width, uint32 height, uint32 depth, uint32 numMips, uint32 flags,
		            const CEClearValue& optimizedClearValue) : CETexture(format, numMips, flags, optimizedClearValue),
		                                                       Width(width), Height(height), Depth(depth) {

		}

		virtual CETexture3D* AsTexture3D() override {
			return this;
		}

		uint32 GetWidth() const {
			return Width;
		}

		uint32 GetHeight() const {
			return Height;
		}

		uint32 GetDepth() const {
			return Depth;
		}

	private:
		uint32 Width;
		uint32 Height;
		uint32 Depth;
	};
}
