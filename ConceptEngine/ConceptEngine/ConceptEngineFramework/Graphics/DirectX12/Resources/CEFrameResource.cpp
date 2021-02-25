#include "CEFrameResource.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CEFrameResource::CEFrameResource(ID3D12Device* device,
                                 UINT passCount,
                                 UINT objectCount,
                                 UINT waveVertexCount) {
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(commandAllocator.GetAddressOf())
	));

	PassCB = std::make_unique<CEUploadBuffer<PassConstants>>(device, passCount, true);
	ObjectCB = std::make_unique<CEUploadBuffer<CEObjectConstants>>(device, objectCount, true);

	WavesVB = std::make_unique<CEUploadBuffer<CEVertex>>(device, waveVertexCount, false);
}

CEFrameResource::~CEFrameResource() {
}
