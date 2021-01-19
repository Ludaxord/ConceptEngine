#pragma once
#include <cstdint>
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl.h>

#include "CEDirectXDescriptorAllocation.h"

namespace ConceptEngine::GraphicsEngine::DirectX {
	class CEDirectXPipelineStateObject;
	class CEDirectXRootSignature;
	class CEDirectXDevice;
	namespace wrl = Microsoft::WRL;

	struct alignas(16) GenerateMipsCB {
		/*
		 * Texture level of source MIP
		 */
		uint32_t SrcMipLevel;
		/*
		 * Number of OutMips to write: [1-4]
		 */
		uint32_t NumMipLevels;
		/*
		 * Width and height of source texture and even or odd.
		 */
		uint32_t SrcDimension;
		/*
		 * Must apply gamma correction to sRGB textures.
		 */
		uint32_t IsSRGB;
		/*
		 * 1.0 / OutMip1.Dimensions
		 */
		::DirectX::XMFLOAT2 TexelSize;
	};

	namespace GenerateMips {
		enum {
			GenerateMipsCB,
			SrcMip,
			OutMip,
			NumRootParameters
		};

		class CEDirectXGenerateMipsPSO {
		public:
			CEDirectXGenerateMipsPSO(CEDirectXDevice& device);

			std::shared_ptr<CEDirectXRootSignature> GetRootSignature() const {
				return m_rootSignature;
			}

			std::shared_ptr<CEDirectXPipelineStateObject> GetPipelineState() const {
				return m_pipelineState;
			}

			D3D12_CPU_DESCRIPTOR_HANDLE GetDefaultUAV() const {
				return m_defaultUAV.GetDescriptorHandle();
			}

		private:
			std::shared_ptr<CEDirectXRootSignature> m_rootSignature;
			std::shared_ptr<CEDirectXPipelineStateObject> m_pipelineState;
			/*
			 * Default (no resource) UAV to pad unused UAV descriptors.
			 * If generating less than 4 mip map levels
			 */
			CEDirectXDescriptorAllocation m_defaultUAV;
		};
	}
}
