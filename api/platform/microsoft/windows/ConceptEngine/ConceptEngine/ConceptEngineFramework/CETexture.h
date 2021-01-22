#pragma once
#include <wrl.h>

#include "CEDescriptorAllocation.h"
#include "CEResource.h"

namespace Concept::GraphicsEngine::Direct3D {
	namespace wrl = Microsoft::WRL;

	class CETexture : public CEResource {
	public:
		/**
		 * resize texture.
		 */
		void Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize = 1);

		/**
		 * Get RTV for texture
		 */
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;

		/**
		 * Get DSV for texture
		 */
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

		/**
		 * Get default SRV for texture.
		 */
		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;

		/**
		 * Get UAV for texture at specific mip level.
		 * Note: Only supports 1D and 2D textures.
		 */
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mip) const;

		bool CheckSRVSupport() const {
			return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
		}

		bool CheckRTVSupport() const {
			return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
		}

		bool CheckUAVSupport() const {
			return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
				CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
				CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
		}

		bool CheckDSVSupport() const {
			return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
		}

		/**
		 * Check if image format has alpha channel
		 */
		bool HasAlpha() const;

		/**
		 * Check number of bits per pixel
		 */
		size_t BitsPerPixel() const;

		static bool IsUAVCompatibleFormat(DXGI_FORMAT format);
		static bool IsSRGBFormat(DXGI_FORMAT format);
		static bool IsBGRFormat(DXGI_FORMAT format);
		static bool IsDepthFormat(DXGI_FORMAT format);

		/**
		 * return typeless format from given format;
		 */
		static DXGI_FORMAT GetTypelessFormat(DXGI_FORMAT format);

		/**
		 * Return sRGB format in same format family.
		 */
		static DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetUAVCompatibleFormat(DXGI_FORMAT format);

	protected:
		CETexture(CEDevice& device, const D3D12_RESOURCE_DESC& resourceDesc,
		                 const D3D12_CLEAR_VALUE* clearValue = nullptr);
		CETexture(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource,
		                 const D3D12_CLEAR_VALUE* clearValue = nullptr);
		virtual ~CETexture();

		/**
		 * Create SRV and UAV for resource.
		 */
		void CreateViews();
	private:
		CEDescriptorAllocation m_renderTargetView;
		CEDescriptorAllocation m_depthStencilView;
		CEDescriptorAllocation m_shaderResourceView;
		CEDescriptorAllocation m_unorderedAccessView;

	};
}
