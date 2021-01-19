#pragma once
#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "CEDescriptorAllocation.h"

namespace ConceptEngine::GraphicsEngine::Direct3D {
	class CEPipelineStateObject;
	class CERootSignature;
	class CEDevice;
	namespace wrl = Microsoft::WRL;
	/*
	 * struct used in PanoToCubemap_CS compute shader;
	 */
	struct PanoToCubemapCB {
		/*
		 * size of cubemap face in pixels at current mipmap level
		 */
		uint32_t CubemapSize;
		/*
		 * first mip level to generate
		 */
		uint32_t FirstMip;
		/*
		 * number of mips to generate
		 */
		uint32_t NumMips;
	};

	namespace PanoToCubemapRS {
		enum {
			PanoToCubemapCB,
			SrcTexture,
			DstMips,
			NumRootParameters
		};

		class CEPanoToCubemapPSO {
		public:
			CEPanoToCubemapPSO(CEDevice& device);

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

			/*
			 * Default (no resource) UAV to pad unused UAV descriptors.
			 * If generating less than 5 mip map levels, unused mip maps
			 * need to be padded with default UAV.
			 */
			CEDescriptorAllocation m_defaultUAV;
		};
	}
}
