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
		CEFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT modelObjectCount, UINT materialCount, bool modelLoaded);
		
		CEFrameResource(const CEFrameResource& rhs) = delete;
		CEFrameResource& operator=(const CEFrameResource& rhs) = delete;
		~CEFrameResource();

		//We cannot reset allocator until GPU is done processing commands
		//so each frame needs their own allocator
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

		//We cannot update cbuffer until GPU is done processing commands
		//that is reference it. So each frame need their own cbuffers
		std::unique_ptr<CEUploadBuffer<PassConstants>> PassCB = nullptr;
		std::unique_ptr<CEUploadBuffer<PassStructuredConstants>> PassStructuredCB = nullptr;
		std::unique_ptr<CEUploadBuffer<PassShadowConstants>> PassShadowCB = nullptr;
		std::unique_ptr<CEUploadBuffer<PassSSAOConstants>> PassSSAOCB = nullptr;
		
		std::unique_ptr<CEUploadBuffer<MaterialConstants>> MaterialCB = nullptr;
		std::unique_ptr<CEUploadBuffer<MaterialData>> MaterialBuffer = nullptr;
		std::unique_ptr<CEUploadBuffer<MaterialIndexData>> MaterialIndexBuffer = nullptr;

		std::unique_ptr<CEUploadBuffer<ObjectConstants>> ObjectCB = nullptr;
		std::unique_ptr<CEUploadBuffer<StructuredObjectConstants>> ObjectStructuredCB = nullptr;

		std::unique_ptr<CEUploadBuffer<SSAOConstants>> SSAOCB = nullptr;
		std::unique_ptr<CEUploadBuffer<ModelConstants>> ModelCB = nullptr;

		//We cannot update a dynamic vertex buffer until GPU is done processing commands that reference it.
		//So each frame needs their own.
		std::unique_ptr<CEUploadBuffer<CEVertex>> WavesVB = nullptr;
		std::unique_ptr<CEUploadBuffer<CENormalVertex>> NormalWavesVB = nullptr;
		std::unique_ptr<CEUploadBuffer<CENormalTextureVertex>> NormalTextureWavesVB = nullptr;

		// NOTE: In this demo, we instance only one render-item, so we only have one structured buffer to 
		// store instancing data.  To make this more general (i.e., to support instancing multiple render-items), 
		// you would need to have a structured buffer for each render-item, and allocate each buffer with enough
		// room for the maximum number of instances you would ever draw.  
		// This sounds like a lot, but it is actually no more than the amount of per-object constant data we 
		// would need if we were not using instancing.  For example, if we were drawing 1000 objects without instancing,
		// we would create a constant buffer with enough room for a 1000 objects.  With instancing, we would just
		// create a structured buffer large enough to store the instance data for 1000 instances.  
		std::unique_ptr<CEUploadBuffer<InstanceData>> InstanceBuffer = nullptr;

		//Fence value to mark commands upt to this fence point.
		//This lets us check if these frame resources are still in use by GPU
		UINT64 Fence = 0;
	protected:
	private:
	};
}
