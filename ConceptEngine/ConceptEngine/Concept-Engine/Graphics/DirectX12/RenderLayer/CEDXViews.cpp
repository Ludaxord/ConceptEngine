#include "CEDXViews.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

CEDXView::CEDXView(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap) : CEDXDeviceElement(device), Resource(nullptr),
                                                                          Heap(heap), OfflineHandle({0}) {
	Assert(Heap != nullptr);
}

CEDXView::~CEDXView() {
	Heap->Free(OfflineHandle, OfflineHeapIndex);
}

bool CEDXView::Create() {
	OfflineHandle = Heap->Allocate(OfflineHeapIndex);
	return OfflineHandle != 0;
}

CEDXConstantBufferView::CEDXConstantBufferView(CEDXDevice* device,
                                               CEDXOfflineDescriptorHeap* heap): CEDXView(device, heap), Desc() {
}

bool CEDXConstantBufferView::CreateView(CEDXResource* resource, const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc) {
	Assert(OfflineHandle != 0);

	Resource = MakeSharedRef<CEDXResource>(resource);
	Desc = desc;

	GetDevice()->CreateConstantBufferView(&Desc, OfflineHandle);

	return true;
}

CEDXBaseShaderResourceView::CEDXBaseShaderResourceView(CEDXDevice* device,
                                                       CEDXOfflineDescriptorHeap* heap): CEDXView(device, heap),
	Desc() {
}

bool CEDXBaseShaderResourceView::CreateView(CEDXResource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) {
	Assert(OfflineHandle != 0);

	Resource = MakeSharedRef<CEDXResource>(resource);
	Desc = desc;

	ID3D12Resource* nativeResource = nullptr;
	if (CEDXView::Resource) {
		nativeResource = CEDXView::Resource->GetResource();
	}

	GetDevice()->CreateShaderResourceView(nativeResource, &Desc, OfflineHandle);

	return true;
}

CEDXBaseUnorderedAccessView::CEDXBaseUnorderedAccessView(CEDXDevice* device,
                                                         CEDXOfflineDescriptorHeap* heap): CEDXView(device, heap),
	Desc(), CounterResource(nullptr) {
}

bool CEDXBaseUnorderedAccessView::CreateView(CEDXResource* counterResource, CEDXResource* resource,
                                             const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc) {
	Assert(OfflineHandle != 0);

	Resource = MakeSharedRef<CEDXResource>(resource);
	CounterResource = counterResource;
	Desc = desc;

	ID3D12Resource* nativeCounterResource = nullptr;
	if (CounterResource) {
		nativeCounterResource = CounterResource->GetResource();
	}

	ID3D12Resource* nativeResource = nullptr;
	if (CEDXView::Resource) {
		nativeResource = CEDXView::Resource->GetResource();
	}

	GetDevice()->CreateUnorderedAccessView(nativeResource, nativeCounterResource, &Desc, OfflineHandle);

	return true;

}

CEDXBaseRenderTargetView::CEDXBaseRenderTargetView(CEDXDevice* device,
                                                   CEDXOfflineDescriptorHeap* heap): CEDXView(device, heap), Desc() {
}

bool CEDXBaseRenderTargetView::CreateView(CEDXResource* resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc) {
	Assert(resource != nullptr);
	Assert(OfflineHandle != 0);

	Resource = MakeSharedRef<CEDXResource>(resource);
	Desc = desc;

	GetDevice()->CreateRenderTargetView(Resource->GetResource(), &Desc, OfflineHandle);

	return true;
}

CEDXBaseDepthStencilView::CEDXBaseDepthStencilView(CEDXDevice* device,
                                                   CEDXOfflineDescriptorHeap* heap): CEDXView(device, heap), Desc() {
}

bool CEDXBaseDepthStencilView::CreateView(CEDXResource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc) {
	Assert(resource != nullptr);
	Assert(OfflineHandle != 0);

	Resource = MakeSharedRef<CEDXResource>(resource);
	Desc = desc;

	GetDevice()->CreateDepthStencilView(Resource->GetResource(), &Desc, OfflineHandle);

	return true;
}
