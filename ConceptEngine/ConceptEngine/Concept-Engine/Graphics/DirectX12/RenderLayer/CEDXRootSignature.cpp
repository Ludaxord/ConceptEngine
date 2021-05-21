#include "CEDXRootSignature.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

static D3D12_SHADER_VISIBILITY GetDXShaderVisibility(uint32 visibility) {
	static D3D12_SHADER_VISIBILITY shaderVisibility[ShaderVisibility_Count] = {
		D3D12_SHADER_VISIBILITY_ALL,
		D3D12_SHADER_VISIBILITY_VERTEX,
		D3D12_SHADER_VISIBILITY_HULL,
		D3D12_SHADER_VISIBILITY_DOMAIN,
		D3D12_SHADER_VISIBILITY_GEOMETRY,
		D3D12_SHADER_VISIBILITY_PIXEL,
	};

	Assert(visibility < ShaderVisibility_Count);
	return shaderVisibility[visibility];
}

static D3D12_STATIC_SAMPLER_DESC GetStaticSamplers(CEStaticSamplers staticSamplers) {
	switch (staticSamplers) {
	case CEStaticSamplers::AnisotropicClamp:
		const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressW
			0.0f, // mipLODBias
			8); // maxAnisotropy
		return anisotropicClamp;
	case CEStaticSamplers::AnisotropicWrap:
		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressW
			0.0f, // mipLODBias
			8); // maxAnisotropy
		return anisotropicWrap;
	case CEStaticSamplers::LinearClamp:
		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW
		return linearClamp;
	case CEStaticSamplers::LinearWrap:
		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW
		return linearWrap;
	case CEStaticSamplers::PointClamp:
		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW
		return pointClamp;
	case CEStaticSamplers::PointWrap:
		const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW
		return pointWrap;
	case CEStaticSamplers::Shadow:
		const CD3DX12_STATIC_SAMPLER_DESC shadow(
			6, // shaderRegister
			D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_BORDER, // addressU
			D3D12_TEXTURE_ADDRESS_MODE_BORDER, // addressV
			D3D12_TEXTURE_ADDRESS_MODE_BORDER, // addressW
			0.0f, // mipLODBias
			16, // maxAnisotropy
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);
		return shadow;
	case CEStaticSamplers::All:
		break;
	}
	return {};
}

static CEShaderVisibility GetShaderVisibility(uint32 visibility) {
	static CEShaderVisibility shaderVisibility[ShaderVisibility_Count] = {
		ShaderVisibility_All,
		ShaderVisibility_Vertex,
		ShaderVisibility_Hull,
		ShaderVisibility_Domain,
		ShaderVisibility_Geometry,
		ShaderVisibility_Pixel
	};

	Assert(visibility < ShaderVisibility_Count);
	return shaderVisibility[visibility];
}

static CEResourceType GetResourceType(D3D12_DESCRIPTOR_RANGE_TYPE type) {
	switch (type) {
	case D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV: return ResourceType_CBV;
	case D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV: return ResourceType_SRV;
	case D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV: return ResourceType_UAV;
	case D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER: return ResourceType_Sampler;
	default:
		Assert(false);
		return ResourceType_Unknown;
	}
}

bool CEDXRootSignatureResourceCount::IsCompatible(const CEDXRootSignatureResourceCount& another) const {
	if (Type != another.Type || AllowInputAssembler != another.AllowInputAssembler) {
		return false;
	}

	for (uint32 i = 0; i < ShaderVisibility_Count; i++) {
		if (!ResourceCounts[i].IsCompatible(another.ResourceCounts[i])) {
			return false;
		}
	}

	return true;
}

CEDXRootSignatureDescHelper::CEDXRootSignatureDescHelper(const CEDXRootSignatureResourceCount& rootSignatureInfo):
	Desc(), Parameters(), DescriptorRanges(), NumDescriptorRanges(0) {
	D3D12_ROOT_SIGNATURE_FLAGS dxRootSignatureFlags[ShaderVisibility_Count] = {
		D3D12_ROOT_SIGNATURE_FLAG_NONE,
		D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS,
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS,
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS,
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS,
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS,
	};

	uint32 space = rootSignatureInfo.Type == CERootSignatureType::RayTracingLocal
		               ? D3D12_SHADER_REGISTER_SPACE_RT_LOCAL
		               : 0;
	uint32 numRootParameters = 0;
	for (uint32 i = 0; i < ShaderVisibility_Count; i++) {
		bool addFlag = true;
		const CEShaderResourceCount& resourceCounts = rootSignatureInfo.ResourceCounts[i];
		if (resourceCounts.Ranges.NumCBVs > 0) {
			Assert(NumDescriptorRanges < D3D12_MAX_DESCRIPTOR_RANGES);
			Assert(numRootParameters < D3D12_MAX_ROOT_PARAMETERS);

			InitDescriptorRange(DescriptorRanges[NumDescriptorRanges], D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			                    resourceCounts.Ranges.NumCBVs, 0, space);
			InitDescriptorTable(Parameters[numRootParameters], GetDXShaderVisibility(i),
			                    &DescriptorRanges[NumDescriptorRanges], 1);
			NumDescriptorRanges++;
			numRootParameters++;

			addFlag = false;
		}

		if (resourceCounts.Ranges.NumSRVs > 0) {

		}

		if (resourceCounts.Ranges.NumUAVs > 0) {

		}

		if (resourceCounts.Ranges.NumSamplers > 0) {

		}

		if (resourceCounts.Num32BitConstants > 0) {

		}

		if (addFlag) {
			Desc.Flags |= dxRootSignatureFlags[i];
		}
	}

	Core::Containers::CEArray<const D3D12_STATIC_SAMPLER_DESC> samplers;
	for (uint8 i = 0; i < CEStaticSamplers::All; ++i) {
		samplers.EmplaceBack(GetStaticSamplers(static_cast<CEStaticSamplers>(i)));
	}

	Desc.NumParameters = numRootParameters;
	Desc.pParameters = Parameters;
	Desc.NumStaticSamplers = samplers.Size();
	if (samplers.Size() > 0) {
		Desc.pStaticSamplers = samplers.Data();
	}
	else {
		Desc.pStaticSamplers = nullptr;
	}

	if (rootSignatureInfo.AllowInputAssembler) {
		Desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}
	else if (rootSignatureInfo.Type == CERootSignatureType::RayTracingLocal) {
		Desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
	}

}

void CEDXRootSignatureDescHelper::InitDescriptorRange(
	D3D12_DESCRIPTOR_RANGE& range,
	D3D12_DESCRIPTOR_RANGE_TYPE type,
	uint32 numDescriptors,
	uint32 baseShaderRegister,
	uint32 registerSpace) {
}

void CEDXRootSignatureDescHelper::InitDescriptorTable(
	D3D12_ROOT_PARAMETER& parameter,
	D3D12_SHADER_VISIBILITY shaderVisibility,
	const D3D12_DESCRIPTOR_RANGE* descriptorRanges,
	uint32 numDescriptorRanges) {
}

void CEDXRootSignatureDescHelper::Init32BitConstantRange(D3D12_ROOT_PARAMETER& parameter,
                                                         D3D12_SHADER_VISIBILITY shaderVisibility,
                                                         uint32 num32BitConstants,
                                                         uint32 shaderRegister,
                                                         uint32 registerSpace) {
}

CEDXRootSignature::CEDXRootSignature(CEDXDevice* device): CEDXDeviceElement(device), RootSignature(nullptr),
                                                          RootParameterMap{},
                                                          ConstantRootParameterIndex(-1) {
}

bool CEDXRootSignature::Create(const CEDXRootSignatureResourceCount& rootSignatureInfo) {
}

bool CEDXRootSignature::Create(const D3D12_ROOT_SIGNATURE_DESC& desc) {
}

bool CEDXRootSignature::Create(const void* blobWithRootSignature, uint64 blobLengthInBytes) {
}

bool CEDXRootSignature::Serialize(const D3D12_ROOT_SIGNATURE_DESC& desc, ID3DBlob** blob) {
}

void CEDXRootSignature::CreateRootParameterMap(const D3D12_ROOT_SIGNATURE_DESC& desc) {
}

bool CEDXRootSignature::InternalCreate(const void* blobWithRootSignature, uint64 blobLengthInBytes) {
}

CEDXRootSignatureCache::CEDXRootSignatureCache(CEDXDevice* device) : CEDXDeviceElement(device), RootSignatures(),
                                                                     ResourceCounts() {
	Instance = this;
}

CEDXRootSignatureCache::~CEDXRootSignatureCache() {
	ReleaseAll();
	Instance = nullptr;
}

bool CEDXRootSignatureCache::Create() {
}

void CEDXRootSignatureCache::ReleaseAll() {
}

CEDXRootSignature* CEDXRootSignatureCache::GetRootSignature(const CEDXRootSignatureResourceCount& resourceCount) {
}

CEDXRootSignatureCache& CEDXRootSignatureCache::Get() {
}

CEDXRootSignature* CEDXRootSignatureCache::CreateRootSignature(const CEDXRootSignatureResourceCount& resourceCount) {
}
