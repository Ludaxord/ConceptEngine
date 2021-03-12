#include "CEFrameResource.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CEFrameResource::CEFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount) {
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(commandAllocator.GetAddressOf())
	));

	commandAllocator->SetName(L"Frame Resource Command Allocator");

	PassCB = std::make_unique<CEUploadBuffer<PassConstants>>(device, passCount, true);
	ObjectCB = std::make_unique<CEUploadBuffer<ObjectConstants>>(device, objectCount, true);
}

CEFrameResource::CEFrameResource(ID3D12Device* device,
                                 UINT passCount,
                                 UINT objectCount,
                                 UINT waveVertexCount,
                                 WaveType waveType) {
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(commandAllocator.GetAddressOf())
	));

	commandAllocator->SetName(L"Frame Resource Command Allocator");

	PassCB = std::make_unique<CEUploadBuffer<PassConstants>>(device, passCount, true);
	ObjectCB = std::make_unique<CEUploadBuffer<ObjectConstants>>(device, objectCount, true);

	if (waveVertexCount > 0) {
		switch (waveType) {
		case WavesVertex:
			WavesVB = std::make_unique<CEUploadBuffer<CEVertex>>(device, waveVertexCount, false);
			break;
		case WavesNormalVertex:
			NormalWavesVB = std::make_unique<CEUploadBuffer<CENormalVertex>>(device, waveVertexCount, false);
			break;
		case WavesNormalTextureVertex:
			NormalTextureWavesVB = std::make_unique<CEUploadBuffer<CENormalTextureVertex>>(
				device, waveVertexCount, false);
			break;
		}
	}
}

CEFrameResource::CEFrameResource(ID3D12Device* device,
                                 UINT passCount,
                                 UINT objectCount,
                                 UINT materialCount,
                                 UINT wavesCount,
                                 WaveType waveType,
                                 bool materialAsConstantBuffer) {
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(commandAllocator.GetAddressOf())
	));

	commandAllocator->SetName(L"Frame Resource Command Allocator");

	PassCB = std::make_unique<CEUploadBuffer<PassConstants>>(device, passCount, true);
	MaterialCB = std::make_unique<CEUploadBuffer<MaterialConstants>>(device, materialCount,
	                                                                 materialAsConstantBuffer
	);
	ObjectCB = std::make_unique<CEUploadBuffer<ObjectConstants>>(device, objectCount, true);

	if (wavesCount > 0) {
		switch (waveType) {
		case WavesVertex:
			WavesVB = std::make_unique<CEUploadBuffer<CEVertex>>(device, wavesCount, false);
			break;
		case WavesNormalVertex:
			NormalWavesVB = std::make_unique<CEUploadBuffer<CENormalVertex>>(device, wavesCount, false);
			break;
		case WavesNormalTextureVertex:
			NormalTextureWavesVB = std::make_unique<CEUploadBuffer<CENormalTextureVertex>>(device, wavesCount, false);
			break;
		}
	}
}

CEFrameResource::~CEFrameResource() {
}
