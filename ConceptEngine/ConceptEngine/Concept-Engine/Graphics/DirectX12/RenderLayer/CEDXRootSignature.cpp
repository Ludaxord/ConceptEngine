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

}

void CEDXRootSignatureDescHelper::InitDescriptorRange() {
}

void CEDXRootSignatureDescHelper::InitDescriptorTable() {
}

void CEDXRootSignatureDescHelper::Init32BitConstantRange() {
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
