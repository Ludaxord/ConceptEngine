#pragma once
#include "CEDXDeviceElement.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {

	enum CEShaderVisibility {
		ShaderVisibility_All = 0,
		ShaderVisibility_Vertex = 0,
		ShaderVisibility_Hull = 0,
		ShaderVisibility_Domain = 0,
		ShaderVisibility_Geometry = 0,
		ShaderVisibility_Pixel = 0,
		ShaderVisibility_Count = ShaderVisibility_Pixel + 1,
	};

	enum EResourceType {
		ResourceType_CBV = 0,
		ResourceType_SRV = 1,
		ResourceType_UAV = 2,
		ResourceType_Sampler = 3,
		ResourceType_Count = ResourceType_Sampler + 1,
		ResourceType_Unknown = 5,
	};

	struct CEShaderResourceRange {
		uint32 NumCBVs = 0;
		uint32 NumSRVs = 0;
		uint32 NumUAVs = 0;
		uint32 NumSamplers = 0;
	};

	struct CEShaderResourceCount {
		void Combine(const CEShaderResourceCount& another);
		bool IsCompatible(const CEShaderResourceCount& another) const;

		CEShaderResourceRange Ranges;
		uint32 Num32BitConstants = 0;
	};

	class CEDXBaseShader : public CEDXDeviceElement {

	};
}
