#pragma once
#include "CETypes.h"
#include "CEUploadBuffer.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	/*
	 * Store resources needed for CPU to build command lists for a frame
	 */
	enum WaveType {
		WavesVertex,
		WavesNormalVertex,
		WavesNormalTextureVertex
	};

	struct CEFrameResource {
	public:
		CEFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
		CEFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertexCount,
		                WaveType waveType = WavesVertex);
		CEFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT wavesCount,
		                WaveType waveType = WavesVertex, bool materialAsConstantBuffer = true);

		CEFrameResource(const CEFrameResource& rhs) = delete;
		CEFrameResource& operator=(const CEFrameResource& rhs) = delete;
		~CEFrameResource();

		//We cannot reset allocator until GPU is done processing commands
		//so each frame needs their own allocator
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

		//We cannot update cbuffer until GPU is done processing commands
		//that is reference it. So each frame need their own cbuffers
		std::unique_ptr<CEUploadBuffer<PassConstants>> PassCB = nullptr;
		std::unique_ptr<CEUploadBuffer<MaterialConstants>> MaterialCB = nullptr;
		std::unique_ptr<CEUploadBuffer<MaterialData>> MaterialBuffer = nullptr;
		std::unique_ptr<CEUploadBuffer<ObjectConstants>> ObjectCB = nullptr;

		//We cannot update a dynamic vertex buffer until GPU is done processing commands that reference it.
		//So each frame needs their own.
		std::unique_ptr<CEUploadBuffer<CEVertex>> WavesVB = nullptr;
		std::unique_ptr<CEUploadBuffer<CENormalVertex>> NormalWavesVB = nullptr;
		std::unique_ptr<CEUploadBuffer<CENormalTextureVertex>> NormalTextureWavesVB = nullptr;

		//Fence value to mark commands upt to this fence point.
		//This lets us check if these frame resources are still in use by GPU
		UINT64 Fence = 0;
	protected:
	private:
	};
}
