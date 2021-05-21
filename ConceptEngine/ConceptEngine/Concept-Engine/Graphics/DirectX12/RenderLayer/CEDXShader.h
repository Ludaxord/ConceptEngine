#pragma once
#include "CEDXDeviceElement.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {

	//TODO: create static sampler class right not put enums here:
	enum CEStaticSampler {
		
	};
	
	enum CEShaderVisibility {
		ShaderVisibility_All = 0,
		ShaderVisibility_Vertex = 1,
		ShaderVisibility_Hull = 2,
		ShaderVisibility_Domain = 3,
		ShaderVisibility_Geometry = 4,
		ShaderVisibility_Pixel = 5,
		ShaderVisibility_Count = ShaderVisibility_Pixel + 1,
	};

	enum CEResourceType {
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
