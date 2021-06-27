#include "CEDXResource.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

CEDXResource::CEDXResource(CEDXDevice* device,
                           const Microsoft::WRL::ComPtr<ID3D12Resource>& nativeResource):
	CERefCountedObject(),
	CEDXDeviceElement(device),
	DXResource(nativeResource),
	HeapType(D3D12_HEAP_TYPE_DEFAULT),
	ResourceState(D3D12_RESOURCE_STATE_COMMON),
	Desc(),
	Address(0) {
}

CEDXResource::CEDXResource(CEDXDevice* device,
                           const D3D12_RESOURCE_DESC& desc, D3D12_HEAP_TYPE heapType):
	CERefCountedObject(),
	CEDXDeviceElement(device),
	DXResource(nullptr),
	HeapType(heapType),
	ResourceState(D3D12_RESOURCE_STATE_COMMON),
	Desc(desc),
	Address(0) {
}

bool CEDXResource::Create(D3D12_RESOURCE_STATES initialState,
                          const D3D12_CLEAR_VALUE* optimizedClearValue) {

	D3D12_HEAP_PROPERTIES HeapProperties;
	Memory::CEMemory::Memzero(&HeapProperties);

	HeapProperties.Type = HeapType;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	//TODO: Fix
	HRESULT Result = GetDevice()->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &Desc, initialState,
	                                                      optimizedClearValue, IID_PPV_ARGS(&DXResource));
	if (SUCCEEDED(Result)) {
		if (Desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {
			Address = DXResource->GetGPUVirtualAddress();
		}

		ResourceState = initialState;
		return true;
	}

	CE_LOG_ERROR("[CEDXResource]: Failed to create commited resource");
	return false;
}

void* CEDXResource::Map(uint32 subResource,
                        const D3D12_RANGE* range) {
	void* MappedData = nullptr;
	HRESULT Result = DXResource->Map(subResource, range, &MappedData);

	if (FAILED(Result)) {
		CE_LOG_ERROR("[CEDXResource::Map]: Failed");
		return nullptr;
	}

	return MappedData;
}

void CEDXResource::Unmap(uint32 subResource,
                         const D3D12_RANGE* range) {
	DXResource->Unmap(subResource, range);
}
