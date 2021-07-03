#include "CEDXRootSignature.h"

#include "../../../Core/Debug/CEDebug.h"

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
			Assert(NumDescriptorRanges < D3D12_MAX_DESCRIPTOR_RANGES);
			Assert(numRootParameters < D3D12_MAX_ROOT_PARAMETERS);

			InitDescriptorRange(DescriptorRanges[NumDescriptorRanges], D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			                    resourceCounts.Ranges.NumSRVs, 0, space);
			InitDescriptorTable(Parameters[numRootParameters], GetDXShaderVisibility(i),
			                    &DescriptorRanges[NumDescriptorRanges], 1);
			NumDescriptorRanges++;
			numRootParameters++;

			addFlag = false;
		}

		if (resourceCounts.Ranges.NumUAVs > 0) {
			Assert(NumDescriptorRanges < D3D12_MAX_DESCRIPTOR_RANGES);
			Assert(numRootParameters < D3D12_MAX_ROOT_PARAMETERS);

			InitDescriptorRange(DescriptorRanges[NumDescriptorRanges], D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
			                    resourceCounts.Ranges.NumUAVs, 0, space);
			InitDescriptorTable(Parameters[numRootParameters], GetDXShaderVisibility(i),
			                    &DescriptorRanges[NumDescriptorRanges], 1);
			NumDescriptorRanges++;
			numRootParameters++;

			addFlag = false;
		}

		if (resourceCounts.Ranges.NumSamplers > 0) {
			Assert(NumDescriptorRanges < D3D12_MAX_DESCRIPTOR_RANGES);
			Assert(numRootParameters < D3D12_MAX_ROOT_PARAMETERS);

			InitDescriptorRange(DescriptorRanges[NumDescriptorRanges], D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
			                    resourceCounts.Ranges.NumSamplers, 0, space);
			InitDescriptorTable(Parameters[numRootParameters], GetDXShaderVisibility(i),
			                    &DescriptorRanges[NumDescriptorRanges], 1);
			NumDescriptorRanges++;
			numRootParameters++;

			addFlag = false;
		}

		if (resourceCounts.Num32BitConstants > 0) {
			Assert(NumDescriptorRanges < D3D12_MAX_DESCRIPTOR_RANGES);
			Assert(numRootParameters < D3D12_MAX_ROOT_PARAMETERS);

			Init32BitConstantRange(Parameters[numRootParameters], GetDXShaderVisibility(i),
			                       resourceCounts.Num32BitConstants, 0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS);
			numRootParameters++;

			addFlag = false;
		}

		if (addFlag) {
			Desc.Flags |= dxRootSignatureFlags[i];
		}
	}

	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
	for (uint8 i = 0; i < CEStaticSamplers::All; ++i) {
		samplers.emplace_back(GetStaticSamplers(static_cast<CEStaticSamplers>(i)));
	}

	Desc.NumParameters = numRootParameters;
	Desc.pParameters = Parameters;
	//TODO: Check if there is need to remove static samplers for now;
	// Desc.NumStaticSamplers = samplers.size();
	// if (!samplers.empty()) {
	// Desc.pStaticSamplers = samplers.data();
	// }
	// else {
	Desc.NumStaticSamplers = 0;
	Desc.pStaticSamplers = nullptr;
	// }

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
	range.BaseShaderRegister = baseShaderRegister;
	range.NumDescriptors = numDescriptors;
	range.RangeType = type;
	range.RegisterSpace = registerSpace;
	range.OffsetInDescriptorsFromTableStart = 0;
}

void CEDXRootSignatureDescHelper::InitDescriptorTable(
	D3D12_ROOT_PARAMETER& parameter,
	D3D12_SHADER_VISIBILITY shaderVisibility,
	const D3D12_DESCRIPTOR_RANGE* descriptorRanges,
	uint32 numDescriptorRanges) {
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter.ShaderVisibility = shaderVisibility;
	parameter.DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
	parameter.DescriptorTable.pDescriptorRanges = descriptorRanges;
}

void CEDXRootSignatureDescHelper::Init32BitConstantRange(
	D3D12_ROOT_PARAMETER& parameter,
	D3D12_SHADER_VISIBILITY shaderVisibility,
	uint32 num32BitConstants,
	uint32 shaderRegister,
	uint32 registerSpace) {
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	parameter.ShaderVisibility = shaderVisibility;
	parameter.Constants.Num32BitValues = num32BitConstants;
	parameter.Constants.ShaderRegister = shaderRegister;
	parameter.Constants.RegisterSpace = registerSpace;
}

CEDXRootSignature::CEDXRootSignature(CEDXDevice* device): CEDXDeviceElement(device), RootSignature(nullptr),
                                                          RootParameterMap(),
                                                          ConstantRootParameterIndex(-1) {
	constexpr uint32 numElements = sizeof(RootParameterMap) / sizeof(uint32);
	int32* ptr = reinterpret_cast<int32*>(&RootParameterMap);
	for (uint32 i = 0; i < numElements; i++) {
		*(ptr++) = -1;
	}
}

bool CEDXRootSignature::Create(const CEDXRootSignatureResourceCount& rootSignatureInfo) {
	CEDXRootSignatureDescHelper desc(rootSignatureInfo);
	return Create(desc.GetDesc());
}

bool CEDXRootSignature::Create(const D3D12_ROOT_SIGNATURE_DESC& desc) {
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	if (!Serialize(desc, &signatureBlob)) {
		CE_LOG_ERROR("[CEDXRootSignature]: Failed to Serialize From Root Signature Desc")
		return false;
	}

	CreateRootParameterMap(desc);

	return InternalCreate(signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize());
}

bool CEDXRootSignature::Create(const void* blobWithRootSignature, uint64 blobLengthInBytes) {
	Microsoft::WRL::ComPtr<ID3D12RootSignatureDeserializer> deserializer;
	HRESULT hResult = Base::CEDirectXHandler::CED3D12CreateRootSignatureDeserializer(
		blobWithRootSignature, blobLengthInBytes, IID_PPV_ARGS(&deserializer));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXRootSignature]: Failed to Retrive Root Signature Desc");
		CEDebug::DebugBreak();
		return false;
	}


	const D3D12_ROOT_SIGNATURE_DESC* desc = deserializer->GetRootSignatureDesc();
	Assert(desc != nullptr);

	CreateRootParameterMap(*desc);

	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	if (!Serialize(*desc, &blob)) {
		CE_LOG_ERROR("[CEDXRootSignature]: Failed to Serialize From Blob")
		return false;
	}

	hResult = GetDevice()->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(),
	                                           IID_PPV_ARGS(&RootSignature));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXRootSignature]: Failed to Create Root Signature");
		CEDebug::DebugBreak();
		return false;
	}

	return InternalCreate(blobWithRootSignature, blobLengthInBytes);
}

bool CEDXRootSignature::Serialize(const D3D12_ROOT_SIGNATURE_DESC& desc, ID3DBlob** blob) {
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	HRESULT hResult = Base::CEDirectXHandler::CED3D12SerializeRootSignature(
		&desc, D3D_ROOT_SIGNATURE_VERSION_1, blob, &errorBlob);
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXRootSignature]: Failed to Serialize RootSignature " + HResultToString(hResult));
		CE_LOG_ERROR(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
		return false;
	}

	return true;
}

void CEDXRootSignature::CreateRootParameterMap(const D3D12_ROOT_SIGNATURE_DESC& desc) {
	for (uint32 i = 0; i < desc.NumParameters; i++) {
		const D3D12_ROOT_PARAMETER& parameter = desc.pParameters[i];
		if (parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			uint32 shaderVisibility = GetShaderVisibility(parameter.ShaderVisibility);

			Assert(parameter.DescriptorTable.NumDescriptorRanges == 1);
			D3D12_DESCRIPTOR_RANGE range = parameter.DescriptorTable.pDescriptorRanges[0];

			uint32 resourceType = GetResourceType(range.RangeType);
			RootParameterMap[shaderVisibility][resourceType] = i;
		}
		else if (parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS) {
			Assert(ConstantRootParameterIndex == -1);
			ConstantRootParameterIndex = i;
		}
	}
}

bool CEDXRootSignature::InternalCreate(const void* blobWithRootSignature, uint64 blobLengthInBytes) {
	HRESULT hResult = GetDevice()->CreateRootSignature(1, blobWithRootSignature, blobLengthInBytes,
	                                                   IID_PPV_ARGS(&RootSignature));
	if (FAILED(hResult)) {
		CE_LOG_ERROR("[CEDXRootSignature]: Failed to create Root Signature");
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

CEDXRootSignatureCache* CEDXRootSignatureCache::Instance = nullptr;

CEDXRootSignatureCache::CEDXRootSignatureCache(CEDXDevice* device) : CEDXDeviceElement(device), RootSignatures(),
                                                                     ResourceCounts() {
	Instance = this;
}

CEDXRootSignatureCache::~CEDXRootSignatureCache() {
	ReleaseAll();
	Instance = nullptr;
}

bool CEDXRootSignatureCache::Create() {
	CEDXRootSignatureResourceCount graphicsKey;
	graphicsKey.Type = CERootSignatureType::Graphics;
	graphicsKey.AllowInputAssembler = true;
	graphicsKey.ResourceCounts[ShaderVisibility_All].Num32BitConstants = D3D12_MAX_32BIT_SHADER_CONSTANTS_COUNT;

	for (uint32 i = 1; i < ShaderVisibility_Count; i++) {
		graphicsKey.ResourceCounts[i].Ranges.NumCBVs = D3D12_DEFAULT_CONSTANT_BUFFER_COUNT;
		graphicsKey.ResourceCounts[i].Ranges.NumSRVs = D3D12_DEFAULT_SHADER_RESOURCE_VIEW_COUNT;
		graphicsKey.ResourceCounts[i].Ranges.NumUAVs = D3D12_DEFAULT_UNORDERED_ACCESS_VIEW_COUNT;
		graphicsKey.ResourceCounts[i].Ranges.NumSamplers = D3D12_DEFAULT_SAMPLER_STATE_COUNT;
	}

	CEDXRootSignature* graphicsRootSignature = CreateRootSignature(graphicsKey);
	if (!graphicsRootSignature) {
		CE_LOG_ERROR("[CEDXRootSignatureCache]: Failed to Create Graphics Root Signature")
		return false;
	}

	CE_LOG_INFO("[CEDXRootSignatureCache]: Create Graphics Root Signature")

	graphicsRootSignature->SetName("Default Graphics Root Signature");

	CEDXRootSignatureResourceCount computeKey;
	computeKey.Type = CERootSignatureType::Compute;
	computeKey.AllowInputAssembler = false;
	computeKey.ResourceCounts[ShaderVisibility_All].Num32BitConstants = D3D12_MAX_32BIT_SHADER_CONSTANTS_COUNT;
	computeKey.ResourceCounts[ShaderVisibility_All].Ranges.NumCBVs = D3D12_DEFAULT_CONSTANT_BUFFER_COUNT;
	computeKey.ResourceCounts[ShaderVisibility_All].Ranges.NumSRVs = D3D12_DEFAULT_SHADER_RESOURCE_VIEW_COUNT;
	computeKey.ResourceCounts[ShaderVisibility_All].Ranges.NumUAVs = D3D12_DEFAULT_UNORDERED_ACCESS_VIEW_COUNT;
	computeKey.ResourceCounts[ShaderVisibility_All].Ranges.NumSamplers = D3D12_DEFAULT_SAMPLER_STATE_COUNT;

	CEDXRootSignature* computeRootSignature = CreateRootSignature(computeKey);
	if (!computeRootSignature) {
		CE_LOG_ERROR("[CEDXRootSignatureCache]: Failed to Create Compute Root Signature")
		return false;
	}

	CE_LOG_INFO("[CEDXRootSignatureCache]: Create Compute Root Signature")

	computeRootSignature->SetName("Default Compute Root Signature");

	if (GetDevice()->GetRayTracingTier() == D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
		return true;
	}

	CEDXRootSignatureResourceCount rtGlobalKey;
	rtGlobalKey.Type = CERootSignatureType::RayTracingGlobal;
	rtGlobalKey.AllowInputAssembler = false;
	rtGlobalKey.ResourceCounts[ShaderVisibility_All].Num32BitConstants = D3D12_MAX_32BIT_SHADER_CONSTANTS_COUNT;
	rtGlobalKey.ResourceCounts[ShaderVisibility_All].Ranges.NumCBVs = D3D12_DEFAULT_CONSTANT_BUFFER_COUNT;
	rtGlobalKey.ResourceCounts[ShaderVisibility_All].Ranges.NumSRVs = D3D12_DEFAULT_SHADER_RESOURCE_VIEW_COUNT;
	rtGlobalKey.ResourceCounts[ShaderVisibility_All].Ranges.NumUAVs = D3D12_DEFAULT_UNORDERED_ACCESS_VIEW_COUNT;
	rtGlobalKey.ResourceCounts[ShaderVisibility_All].Ranges.NumSamplers = D3D12_DEFAULT_SAMPLER_STATE_COUNT;

	CEDXRootSignature* rtGlobalRootSignature = CreateRootSignature(rtGlobalKey);
	if (!rtGlobalRootSignature) {
		return false;
	}

	rtGlobalRootSignature->SetName("Default Global Ray Tracing RootSignature");

	CEDXRootSignatureResourceCount rtLocalKey;
	rtLocalKey.Type = CERootSignatureType::RayTracingLocal;
	rtLocalKey.AllowInputAssembler = false;
	rtLocalKey.ResourceCounts[ShaderVisibility_All].Ranges.NumCBVs = D3D12_DEFAULT_CONSTANT_BUFFER_COUNT;
	rtLocalKey.ResourceCounts[ShaderVisibility_All].Ranges.NumSRVs = D3D12_DEFAULT_SHADER_RESOURCE_VIEW_COUNT;
	rtLocalKey.ResourceCounts[ShaderVisibility_All].Ranges.NumUAVs = D3D12_DEFAULT_UNORDERED_ACCESS_VIEW_COUNT;
	rtLocalKey.ResourceCounts[ShaderVisibility_All].Ranges.NumSamplers = D3D12_DEFAULT_SAMPLER_STATE_COUNT;

	CEDXRootSignature* rtLocalRootSignature = CreateRootSignature(rtLocalKey);
	if (!rtLocalRootSignature) {
		return false;
	}

	rtLocalRootSignature->SetName("Default Local Ray Tracing RootSignature");

	return true;
}

void CEDXRootSignatureCache::ReleaseAll() {
	for (CERef<CEDXRootSignature> rootSignature : RootSignatures) {
		rootSignature.Reset();
	}

	RootSignatures.Clear();
	ResourceCounts.Clear();
}

CEDXRootSignature* CEDXRootSignatureCache::GetRootSignature(const CEDXRootSignatureResourceCount& resourceCount) {
	Assert(RootSignatures.Size() == ResourceCounts.Size());

	for (uint32 i = 0; i < ResourceCounts.Size(); i++) {
		if (resourceCount.IsCompatible(ResourceCounts[i])) {
			CE_LOG_DEBUG("[CEDXRootSignature] Found Compatible Root Signature at: " + std::to_string(i));
			return RootSignatures[i].Get();
		}
	}

	CEDXRootSignatureResourceCount newResourceCount = resourceCount;
	for (uint32 i = 0; i < ShaderVisibility_Count; i++) {
		CEShaderResourceCount& count = newResourceCount.ResourceCounts[i];
		if (count.Ranges.NumCBVs > 0) {
			count.Ranges.NumCBVs = Math::CEMath::Max<uint32>(count.Ranges.NumCBVs, D3D12_DEFAULT_CONSTANT_BUFFER_COUNT);
		}
		if (count.Ranges.NumSRVs > 0) {
			count.Ranges.NumSRVs = Math::CEMath::Max<uint32>(count.Ranges.NumSRVs,
			                                                 D3D12_DEFAULT_SHADER_RESOURCE_VIEW_COUNT);
		}
		if (count.Ranges.NumUAVs > 0) {
			count.Ranges.NumUAVs = Math::CEMath::Max<uint32>(count.Ranges.NumUAVs,
			                                                 D3D12_DEFAULT_UNORDERED_ACCESS_VIEW_COUNT);
		}
		if (count.Ranges.NumSamplers > 0) {
			count.Ranges.NumSamplers = Math::CEMath::Max<uint32>(count.Ranges.NumSamplers,
			                                                     D3D12_DEFAULT_SAMPLER_STATE_COUNT);
		}
	}

	return CreateRootSignature(newResourceCount);
}

CEDXRootSignatureCache& CEDXRootSignatureCache::Get() {
	Assert(Instance != nullptr);
	return *Instance;
}

CEDXRootSignature* CEDXRootSignatureCache::CreateRootSignature(const CEDXRootSignatureResourceCount& resourceCount) {
	CERef<CEDXRootSignature> rootSignature = new CEDXRootSignature(GetDevice());
	if (!rootSignature->Create(resourceCount)) {
		CE_LOG_ERROR("[CEDXRootSignatureCache] Failed to create Root Signature");
		return nullptr;
	}

	RootSignatures.EmplaceBack(rootSignature);
	ResourceCounts.EmplaceBack(resourceCount);
	return rootSignature.Get();
}
