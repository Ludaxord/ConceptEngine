#include "CEDXDescriptorCache.h"

#include "../../../Core/Application/CECore.h"
#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../Managers/CEDXManager.h"

// Helper Macros
#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

CEDXDescriptorCache::CEDXDescriptorCache(CEDXDevice* device): CEDXDeviceElement(device),
                                                              NullCBV(nullptr),
                                                              NullSRV(nullptr),
                                                              NullUAV(nullptr),
                                                              NullSampler(nullptr),
                                                              ShaderResourceViewCache(),
                                                              UnorderedAccessViewCache(),
                                                              ConstantBufferViewCache(),
                                                              SamplerStateCache(),
                                                              RangeSizes() {
}

CEDXDescriptorCache::~CEDXDescriptorCache() {
	if (NullCBV) {
		delete NullCBV;
		NullCBV = nullptr;
	}
	if (NullSRV) {
		delete NullSRV;
		NullSRV = nullptr;
	}
	if (NullUAV) {
		delete NullUAV;
		NullUAV = nullptr;
	}
	if (NullSampler) {
		delete NullSampler;
		NullSampler = nullptr;
	}
}

bool CEDXDescriptorCache::Create() {
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	Memory::CEMemory::Memzero(&cbvDesc);

	cbvDesc.BufferLocation = 0;
	cbvDesc.SizeInBytes = 0;
	NullCBV = new CEDXConstantBufferView(GetDevice(), CastDXManager()->GetResourceOfflineDescriptorHeap());
	if (!NullCBV->Create()) {
		return false;
	}

	if (!NullCBV->CreateView(nullptr, cbvDesc)) {
		return false;
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	Memory::CEMemory::Memzero(&uavDesc);

	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.Texture2D.MipSlice = 0;
	uavDesc.Texture2D.PlaneSlice = 0;

	NullUAV = new CEDXUnorderedAccessView(GetDevice(), CastDXManager()->GetResourceOfflineDescriptorHeap()
	);
	if (!NullUAV->Create()) {
		return false;
	}

	if (!NullUAV->CreateView(nullptr, nullptr, uavDesc)) {
		return false;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	Memory::CEMemory::Memzero(&srvDesc);

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;

	NullSRV = new CEDXShaderResourceView(GetDevice(), CastDXManager()->GetResourceOfflineDescriptorHeap()
	);
	if (!NullSRV->Create()) {
		return false;
	}

	if (!NullSRV->CreateView(nullptr, srvDesc)) {
		return false;
	}

	D3D12_SAMPLER_DESC samplerDesc;
	Memory::CEMemory::Memzero(&samplerDesc);

	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MaxLOD = FLT_MAX;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MipLODBias = 0.0f;

	NullSampler = new CEDXSamplerState(GetDevice(), CastDXManager()->GetSamplerOfflineDescriptorHeap());

	if (!NullSampler->Create(samplerDesc)) {
		return false;
	}

	for (uint32 i = 0; i < NUM_DESCRIPTORS; i++) {
		RangeSizes[i] = 1;
	}

	return true;
}

void CEDXDescriptorCache::CommitGraphicsDescriptors(CEDXCommandListHandle& commandList, CEDXCommandBatch* commandBatch,
                                                    CEDXRootSignature* rootSignature) {
	Assert(commandBatch != nullptr);
	Assert(rootSignature != nullptr);

	VertexBufferCache.CommitState(commandList);
	RenderTargetCache.CommitState(commandList);

	ID3D12GraphicsCommandList* dxCommandList = commandList.GetGraphicsCommandList();
	CEDXOnlineDescriptorHeap* resourceHeap = commandBatch->GetOnlineResourceDescriptorHeap();
	CEDXOnlineDescriptorHeap* samplerHeap = commandBatch->GetOnlineResourceDescriptorHeap();

	CopyDescriptorsAndSetHeaps(dxCommandList, resourceHeap, samplerHeap);

	for (uint32 i = 0; i < ShaderVisibility_Count; i++) {
		CEShaderVisibility visibility = (CEShaderVisibility)i;

		uint64 numCBVs = ConstantBufferViewCache.DescriptorRangeLengths[visibility];
		int32 parameterIndex = rootSignature->GetRootParameterIndex(visibility, CEResourceType::ResourceType_CBV);
		if (parameterIndex >= 0 && numCBVs > 0) {
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = ConstantBufferViewCache.Descriptors[visibility];
			dxCommandList->SetGraphicsRootDescriptorTable(parameterIndex, gpuHandle);
		}

		uint64 numSRVs = ShaderResourceViewCache.DescriptorRangeLengths[visibility];
		parameterIndex = rootSignature->GetRootParameterIndex(visibility, CEResourceType::ResourceType_SRV);
		if (parameterIndex >= 0 && numSRVs > 0) {
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = ShaderResourceViewCache.Descriptors[visibility];
			dxCommandList->SetGraphicsRootDescriptorTable(parameterIndex, gpuHandle);
		}

		uint64 numUAVs = UnorderedAccessViewCache.DescriptorRangeLengths[visibility];
		parameterIndex = rootSignature->GetRootParameterIndex(visibility, CEResourceType::ResourceType_UAV);
		if (parameterIndex >= 0 && numUAVs > 0) {
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = UnorderedAccessViewCache.Descriptors[visibility];
			dxCommandList->SetGraphicsRootDescriptorTable(parameterIndex, gpuHandle);
		}

		uint64 numSamplers = SamplerStateCache.DescriptorRangeLengths[visibility];
		parameterIndex = rootSignature->GetRootParameterIndex(visibility, CEResourceType::ResourceType_Sampler);
		if (parameterIndex >= 0 && numSamplers > 0) {
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = SamplerStateCache.Descriptors[visibility];
			dxCommandList->SetGraphicsRootDescriptorTable(parameterIndex, gpuHandle);
		}
	}
}

void CEDXDescriptorCache::CommitComputeDescriptors(CEDXCommandListHandle& commandList, CEDXCommandBatch* commandBatch,
                                                   CEDXRootSignature* rootSignature) {
	Assert(commandBatch != nullptr);
	Assert(rootSignature != nullptr);

	ID3D12GraphicsCommandList* dxCommandList = commandList.GetGraphicsCommandList();
	CEDXOnlineDescriptorHeap* resourceHeap = commandBatch->GetOnlineResourceDescriptorHeap();
	CEDXOnlineDescriptorHeap* samplerHeap = commandBatch->GetOnlineSamplerDescriptorHeap();

	CopyDescriptorsAndSetHeaps(dxCommandList, resourceHeap, samplerHeap);

	CEShaderVisibility visibility = ShaderVisibility_All;

	uint64 numCBVs = ConstantBufferViewCache.DescriptorRangeLengths[visibility];
	int32 parameterIndex = rootSignature->GetRootParameterIndex(visibility, CEResourceType::ResourceType_CBV);
	if (parameterIndex >= 0 && numCBVs > 0) {
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = ConstantBufferViewCache.Descriptors[visibility];
		dxCommandList->SetComputeRootDescriptorTable(parameterIndex, gpuHandle);
	}

	uint64 numSRVs = ShaderResourceViewCache.DescriptorRangeLengths[visibility];
	parameterIndex = rootSignature->GetRootParameterIndex(visibility, CEResourceType::ResourceType_SRV);
	if (parameterIndex >= 0 && numSRVs > 0) {
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = ShaderResourceViewCache.Descriptors[visibility];
		dxCommandList->SetComputeRootDescriptorTable(parameterIndex, gpuHandle);
	}

	uint64 numUAVs = UnorderedAccessViewCache.DescriptorRangeLengths[visibility];
	parameterIndex = rootSignature->GetRootParameterIndex(visibility, CEResourceType::ResourceType_UAV);
	if (parameterIndex >= 0 && numUAVs > 0) {
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = UnorderedAccessViewCache.Descriptors[visibility];
		dxCommandList->SetComputeRootDescriptorTable(parameterIndex, gpuHandle);
	}

	uint64 numSamplers = SamplerStateCache.DescriptorRangeLengths[visibility];
	parameterIndex = rootSignature->GetRootParameterIndex(visibility, CEResourceType::ResourceType_Sampler);
	if (parameterIndex >= 0 && numSamplers > 0) {
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = SamplerStateCache.Descriptors[visibility];
		dxCommandList->SetComputeRootDescriptorTable(parameterIndex, gpuHandle);
	}

}

void CEDXDescriptorCache::Reset() {
	VertexBufferCache.Reset();
	RenderTargetCache.Reset();

	ConstantBufferViewCache.Reset();
	ShaderResourceViewCache.Reset();
	UnorderedAccessViewCache.Reset();
	SamplerStateCache.Reset();

	PreviousDescriptorHeaps[0] = nullptr;
	PreviousDescriptorHeaps[1] = nullptr;
}

void CEDXDescriptorCache::CopyDescriptorsAndSetHeaps(ID3D12GraphicsCommandList* CommandList,
                                                     CEDXOnlineDescriptorHeap* ResourceHeap,
                                                     CEDXOnlineDescriptorHeap* SamplerHeap) {

	uint32 numResourceDescriptors =
		ConstantBufferViewCache.CountNeededDescriptors() +
		ShaderResourceViewCache.CountNeededDescriptors() +
		UnorderedAccessViewCache.CountNeededDescriptors();

	if (!ResourceHeap->HasSpace(numResourceDescriptors)) {
		ResourceHeap->AllocateFreshHeap();

		ConstantBufferViewCache.InvalidateAll();
		ShaderResourceViewCache.InvalidateAll();
		UnorderedAccessViewCache.InvalidateAll();
	}

	ConstantBufferViewCache.PrepareForCopy(NullCBV);
	ShaderResourceViewCache.PrepareForCopy(NullSRV);
	UnorderedAccessViewCache.PrepareForCopy(NullUAV);

	uint32 numSamplerDescriptors = SamplerStateCache.CountNeededDescriptors();
	if (!SamplerHeap->HasSpace(numSamplerDescriptors)) {
		SamplerHeap->AllocateFreshHeap();
		SamplerStateCache.InvalidateAll();
	}

	SamplerStateCache.PrepareForCopy(NullSampler);

	Assert(numResourceDescriptors < D3D12_MAX_ONLINE_DESCRIPTOR_COUNT);
	uint32 resourceDescriptorHandle = ResourceHeap->AllocateHandles(numResourceDescriptors);

	Assert(numSamplerDescriptors < D3D12_MAX_ONLINE_DESCRIPTOR_COUNT);
	uint32 samplerDescriptorHandle = SamplerHeap->AllocateHandles(numSamplerDescriptors);

	ID3D12DescriptorHeap* DescriptorHeaps[] =
	{
		ResourceHeap->GetNativeHeap(),
		SamplerHeap->GetNativeHeap()
	};

	if (PreviousDescriptorHeaps[0] != DescriptorHeaps[0] || PreviousDescriptorHeaps[1] != DescriptorHeaps[1]) {
		CommandList->SetDescriptorHeaps(ArrayCount(DescriptorHeaps), DescriptorHeaps);

		PreviousDescriptorHeaps[0] = DescriptorHeaps[0];
		PreviousDescriptorHeaps[1] = DescriptorHeaps[1];
	}


	ID3D12Device* dxDevice = GetDevice()->GetDevice();
	if (ConstantBufferViewCache.TotalNumDescriptors > 0) {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = ResourceHeap->GetCPUDescriptorHandleAt(resourceDescriptorHandle);
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = ResourceHeap->GetGPUDescriptorHandleAt(resourceDescriptorHandle);

		UINT destDescriptorRangeSize = ConstantBufferViewCache.TotalNumDescriptors;
		dxDevice->CopyDescriptors(1, &cpuHandle, &destDescriptorRangeSize, destDescriptorRangeSize,
		                          ConstantBufferViewCache.CopyDescriptors, RangeSizes,
		                          D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		ConstantBufferViewCache.SetGPUHandles(gpuHandle, ResourceHeap->GetDescriptorHandleIncrementSize());
		resourceDescriptorHandle += destDescriptorRangeSize;
	}
	if (ShaderResourceViewCache.TotalNumDescriptors > 0) {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = ResourceHeap->GetCPUDescriptorHandleAt(resourceDescriptorHandle);
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = ResourceHeap->GetGPUDescriptorHandleAt(resourceDescriptorHandle);

		UINT destDescriptorRangeSize = ShaderResourceViewCache.TotalNumDescriptors;
		dxDevice->CopyDescriptors(1, &cpuHandle, &destDescriptorRangeSize, destDescriptorRangeSize,
		                          ShaderResourceViewCache.CopyDescriptors, RangeSizes,
		                          D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		ShaderResourceViewCache.SetGPUHandles(gpuHandle, ResourceHeap->GetDescriptorHandleIncrementSize());
		resourceDescriptorHandle += destDescriptorRangeSize;
	}
	if (UnorderedAccessViewCache.TotalNumDescriptors > 0) {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = ResourceHeap->GetCPUDescriptorHandleAt(resourceDescriptorHandle);
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = ResourceHeap->GetGPUDescriptorHandleAt(resourceDescriptorHandle);

		UINT destDescriptorRangeSize = UnorderedAccessViewCache.TotalNumDescriptors;
		dxDevice->CopyDescriptors(1, &cpuHandle, &destDescriptorRangeSize, destDescriptorRangeSize,
		                          UnorderedAccessViewCache.CopyDescriptors, RangeSizes,
		                          D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		UnorderedAccessViewCache.SetGPUHandles(gpuHandle, ResourceHeap->GetDescriptorHandleIncrementSize());
		resourceDescriptorHandle += destDescriptorRangeSize;
	}
	if (SamplerStateCache.TotalNumDescriptors > 0) {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = SamplerHeap->GetCPUDescriptorHandleAt(samplerDescriptorHandle);
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = SamplerHeap->GetGPUDescriptorHandleAt(samplerDescriptorHandle);

		UINT destDescriptorRangeSize = SamplerStateCache.TotalNumDescriptors;
		dxDevice->CopyDescriptors(1, &cpuHandle, &destDescriptorRangeSize, destDescriptorRangeSize,
		                          SamplerStateCache.CopyDescriptors, RangeSizes, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		SamplerStateCache.SetGPUHandles(gpuHandle, SamplerHeap->GetDescriptorHandleIncrementSize());
	}
}
