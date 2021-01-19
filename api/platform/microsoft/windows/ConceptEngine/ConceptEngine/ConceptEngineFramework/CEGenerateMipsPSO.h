#pragma once
#include <cstdint>
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl.h>

#include "CEDescriptorAllocation.h"

namespace ConceptEngine::GraphicsEngine::Direct3D {
	class CEPipelineStateObject;
	class CERootSignature;
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	struct alignas(16) GenerateMipsCB {
		/**
		 * Texture level of source MIP
		 */
		uint32_t SrcMipLevel;
		/**
		 * Number of OutMips to write: [1-4]
		 */
		uint32_t NumMipLevels;
		/**
		 * Width and height of source texture and even or odd.
		 */
		uint32_t SrcDimension;
		/**
		 * Must apply gamma correction to sRGB textures.
		 */
		uint32_t IsSRGB;
		/**
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

		class CEGenerateMipsPSO {
		public:
			CEGenerateMipsPSO(CEDevice& device);

			std::shared_ptr<CERootSignature> GetRootSignature() const {
				return m_rootSignature;
			}

			std::shared_ptr<CEPipelineStateObject> GetPipelineState() const {
				return m_pipelineState;
			}

			D3D12_CPU_DESCRIPTOR_HANDLE GetDefaultUAV() const {
				return m_defaultUAV.GetDescriptorHandle();
			}

		private:
			std::shared_ptr<CERootSignature> m_rootSignature;
			std::shared_ptr<CEPipelineStateObject> m_pipelineState;
			/**
			 * Default (no resource) UAV to pad unused UAV descriptors.
			 * If generating less than 4 mip map levels
			 */
			CEDescriptorAllocation m_defaultUAV;
		};
	}
}
