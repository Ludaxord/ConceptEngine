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
	return false;
}

void* CEDXResource::Map(uint32 subResource,
                        const D3D12_RANGE* range) {
	return nullptr;
}

void CEDXResource::Unmap(uint32 subResource,
                         const D3D12_RANGE* range) {
}
