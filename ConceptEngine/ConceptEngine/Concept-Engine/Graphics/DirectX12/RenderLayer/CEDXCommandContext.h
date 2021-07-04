#pragma once

#include <d3d12.h>
#include <boost/function.hpp>
#include <wrl/client.h>

#include "CEDXCommandList.h"
#include "CEDXCommandQueue.h"
#include "CEDXDescriptorCache.h"
#include "CEDXDescriptorHeap.h"
#include "CEDXDevice.h"
#include "CEDXDeviceElement.h"
#include "CEDXFence.h"
#include "CEDXGPUProfiler.h"
#include "CEDXPipelineState.h"
#include "CEDXResource.h"
#include "CEDXRootSignature.h"

#include "../../Main/RenderLayer/CEICommandContext.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	struct CEDXUploadAllocation {
		uint8* MappedPtr = nullptr;
		uint64 ResourceOffset = 0;
	};

	class CEDXGPUResourceUploader : public CEDXDeviceElement {
	public:
		CEDXGPUResourceUploader(CEDXDevice* device);
		~CEDXGPUResourceUploader() = default;

		bool Reserve(uint32 sizeInBytes);
		void Reset();

		CEDXUploadAllocation LinearAllocate(uint32 sizeInBytes);

		ID3D12Resource* GetGPUResource() const {
			return Resource.Get();
		}

		uint32 GetSizeInBytes() const {
			return SizeInBytes;
		}

	private:
		uint8* MappedMemory = nullptr;
		uint32 SizeInBytes = 0;
		uint32 OffsetInBytes = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
		CEArray<Microsoft::WRL::ComPtr<ID3D12Resource>> GarbageResources;
	};

	class CEDXCommandBatch {
	public:
		CEDXCommandBatch(CEDXDevice* device);
		~CEDXCommandBatch() = default;

		bool Create();

		bool Reset() {
			if (CommandAllocator.Reset()) {
				Resources.Clear();
				NativeResources.Clear();
				DXResources.Clear();

				GPUResourceUploader.Reset();

				OnlineResourceDescriptorHeap->Reset();
				OnlineSamplerDescriptorHeap->Reset();

				return true;
			}
			return false;
		}

		void AddInUseResource(CEResource* resource) {
			if (resource) {
				Resources.EmplaceBack(MakeSharedRef<CEResource>(resource));
			}
		}

		void AddInUseResource(CEDXResource* resource) {
			if (resource) {
				DXResources.EmplaceBack(MakeSharedRef<CEDXResource>(resource));
			}
		}

		void AddInUseResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource) {
			if (resource) {
				NativeResources.EmplaceBack(resource);
			}
		}

		CEDXGPUResourceUploader& GetGpuResourceUploader() {
			return GPUResourceUploader;
		}

		CEDXCommandAllocatorHandle& GetCommandAllocator() {
			return CommandAllocator;
		}

		CEDXOnlineDescriptorHeap* GetOnlineResourceDescriptorHeap() const {
			return OnlineResourceDescriptorHeap.Get();
		}

		CEDXOnlineDescriptorHeap* GetOnlineSamplerDescriptorHeap() const {
			return OnlineSamplerDescriptorHeap.Get();
		}


	private:
		CEDXDevice* Device = nullptr;

		CEDXCommandAllocatorHandle CommandAllocator;
		CEDXGPUResourceUploader GPUResourceUploader;

		CERef<CEDXOnlineDescriptorHeap> OnlineResourceDescriptorHeap;
		CERef<CEDXOnlineDescriptorHeap> OnlineSamplerDescriptorHeap;

		CERef<CEDXOnlineDescriptorHeap> OnlineRayTracingResourceDescriptorHeap;
		CERef<CEDXOnlineDescriptorHeap> OnlineRayTracingSamplerDescriptorHeap;

		CEArray<CERef<CEDXResource>> DXResources;
		CEArray<CERef<CEResource>> Resources;
		CEArray<Microsoft::WRL::ComPtr<ID3D12Resource>> NativeResources;
	};

	class CEDXResourceBarrierBatcher {
	public:
		CEDXResourceBarrierBatcher() = default;
		~CEDXResourceBarrierBatcher() = default;

		void AddTransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState,
		                          D3D12_RESOURCE_STATES afterState);

		void AddUnorderedAccessBarrier(ID3D12Resource* resource) {
			Assert(resource != nullptr);
			D3D12_RESOURCE_BARRIER barrier;
			Memory::CEMemory::Memzero(&barrier);

			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrier.UAV.pResource = resource;

			Barriers.EmplaceBack(barrier);
		}

		void FlushBarriers(CEDXCommandListHandle& commandList) {
			if (!Barriers.IsEmpty()) {
				commandList.ResourceBarrier(Barriers.Data(), Barriers.Size());
				Barriers.Clear();
			}
		}

		const D3D12_RESOURCE_BARRIER* GetBarriers() const {
			return Barriers.Data();
		}

		uint32 GetNumBarriers() const {
			return Barriers.Size();
		}

	private:
		CEArray<D3D12_RESOURCE_BARRIER> Barriers;
	};


	class CEDXCommandContext : public ConceptEngine::Graphics::Main::RenderLayer::CEICommandContext,
	                           public CEDXDeviceElement {
	public:
		CEDXCommandContext(CEDXDevice* device);
		~CEDXCommandContext();

		bool Create();

		CEDXCommandQueueHandle& GetHandle() {
			return CommandQueue;
		}

		CEDXCommandListHandle& GetCommandList() {
			return CommandList;
		}

		CEDXCommandQueueHandle& GetCommandQueue() {
			return CommandQueue;
		}

		uint32 GetCurrentEpochValue() const {
			uint32 maxValue = Math::CEMath::Max<int32>((int32)CommandBatches.Size() - 1, 0);
			return Math::CEMath::Min<uint32>(NextCommandBatch - 1, maxValue);
		}

		void UpdateBuffer(CEDXResource* Resource, uint64 offsetInBytes, uint64 sizeInBytes, const void* sourceData);

		void UnorderedAccessBarrier(CEDXResource* resource) {
			BarrierBatcher.AddUnorderedAccessBarrier(resource->GetResource());
		}

		void TransitionResource(CEDXResource* resource, D3D12_RESOURCE_STATES beforeState,
		                        D3D12_RESOURCE_STATES afterState) {
			BarrierBatcher.AddTransitionBarrier(resource->GetResource(), beforeState, afterState);
		}

		void FlushResourceBarriers() {
			BarrierBatcher.FlushBarriers(CommandList);
		}

		void DiscardResource(CEDXResource* resource) {
			CommandBatch->AddInUseResource(resource);
		}

	public:
		virtual void Begin() override final;
		virtual void End() override final;

		void Execute(boost::function<void()> ExecuteFunction) {
			Begin();
			ExecuteFunction();
			End();
		}

		void BeginTimeStamp(CEGPUProfiler* profiler, uint32 index) override;
		void EndTimeStamp(CEGPUProfiler* profiler, uint32 index) override;
		void DispatchRays(CERayTracingScene* rayTracingScene, CERayTracingPipelineState* pipelineState,
		                  uint32 width, uint32 height, uint32 depth) override;
		void SetRayTracingBindings(CERayTracingScene* rayTracingScene,
		                           ConceptEngine::Graphics::Main::RenderLayer::CERayTracingPipelineState* pipelineState,
		                           const CERayTracingShaderResources* globalResource,
		                           const CERayTracingShaderResources* rayGenLocalResources,
		                           const CERayTracingShaderResources* missLocalResources,
		                           const CERayTracingShaderResources* hitGroupResources,
		                           uint32 numHitGroupResources) override;
		void InsertMaker(const std::string& message) override;

		virtual void ClearRenderTargetView(CERenderTargetView* renderTargetView,
		                                   Math::CEColorF& clearColor) override final;
		virtual void ClearDepthStencilView(CEDepthStencilView* depthStencilView,
		                                   const CEDepthStencilF& clearColor) override final;
		virtual void ClearUnorderedAccessViewFloat(CEUnorderedAccessView* unorderedAccessView,
		                                           const Math::CEColorF& clearColor) override final;
		virtual void SetShadingRate(CEShadingRate shadingRate) override final;
		virtual void SetShadingRateImage(CETexture2D* shadingImage) override final;
		virtual void
		SetViewport(float width, float height, float minDepth, float maxDepth, float x, float y) override final;
		virtual void SetScissorRect(float width, float height, float x, float y) override final;
		virtual void SetBlendFactor(const Math::CEColorF& color) override final;
		virtual void BeginRenderPass(const Math::CEColorF& color, CERenderTargetView* renderTargetView,
		                             CEDepthStencilView* depthStencilView) override final;
		virtual void EndRenderPass() override final;
		virtual void
		SetPrimitiveTopology(CEPrimitiveTopology primitiveTopologyType) override final;
		virtual void SetVertexBuffers(CEVertexBuffer* const* vertexBuffers, uint32 bufferCount,
		                              uint32 bufferSlot) override final;
		virtual void SetIndexBuffer(CEIndexBuffer* indexBuffer) override final;
		virtual void SetRenderTargets(CERenderTargetView* const* renderTargetView,
		                              uint32 renderTargetCount,
		                              CEDepthStencilView* depthStencilView) override final;
		virtual void
		SetGraphicsPipelineState(
			ConceptEngine::Graphics::Main::RenderLayer::CEGraphicsPipelineState* pipelineState) override final;
		virtual void
		SetComputePipelineState(
			ConceptEngine::Graphics::Main::RenderLayer::CEComputePipelineState* pipelineState) override final;
		virtual void Set32BitShaderConstants(CEShader* shader, const void* shader32BitConstants,
		                                     uint32 num32BitConstants) override final;
		virtual void SetShaderResourceView(CEShader* shader,
		                                   CEShaderResourceView* shaderResourceView,
		                                   uint32 parameterIndex) override final;
		virtual void SetShaderResourceViews(CEShader* shader,
		                                    CEShaderResourceView* const* shaderResourceView,
		                                    uint32 numShaderResourceViews,
		                                    uint32 parameterIndex) override final;
		virtual void SetUnorderedAccessView(CEShader* shader,
		                                    CEUnorderedAccessView* unorderedAccessView,
		                                    uint32 parameterIndex) override final;
		virtual void SetUnorderedAccessViews(CEShader* shader,
		                                     CEUnorderedAccessView* const* unorderedAccessViews,
		                                     uint32 numUnorderedAccessViews,
		                                     uint32 parameterIndex) override final;
		virtual void SetConstantBuffer(CEShader* shader,
		                               CEConstantBuffer* constantBuffer,
		                               uint32 parameterIndex) override final;
		virtual void SetConstantBuffers(CEShader* shader,
		                                CEConstantBuffer* const* constantBuffers,
		                                uint32 numConstantBuffers,
		                                uint32 parameterIndex) override final;
		virtual void SetSamplerState(CEShader* shader,
		                             ConceptEngine::Graphics::Main::RenderLayer::CESamplerState* samplerState,
		                             uint32 parameterIndex) override final;
		virtual void SetSamplerStates(CEShader* shader,
		                              ConceptEngine::Graphics::Main::RenderLayer::CESamplerState* const* samplerStates,
		                              uint32 numSamplerStates,
		                              uint32 parameterIndex) override final;
		virtual void ResolveTexture(CETexture* destination,
		                            CETexture* source) override final;
		virtual void UpdateBuffer(CEBuffer* destination, uint64 offsetInBytes,
		                          uint64 sizeInBytes,
		                          const void* sourceData) override final;
		virtual void UpdateTexture2D(CETexture2D* destination, uint32 width, uint32 height,
		                             uint32 mipLevel,
		                             const void* sourceData) override final;
		virtual void CopyBuffer(CEBuffer* destination, Main::RenderLayer::CEBuffer* source,
		                        const CECopyBufferInfo& copyInfo) override final;
		virtual void CopyTexture(CETexture* destination,
		                         CETexture* source) override final;
		virtual void CopyTextureRegion(CETexture* destination,
		                               CETexture* source,
		                               const CECopyTextureInfo& copyTexture) override final;
		virtual void DiscardResource(CEResource* resource) override final;
		virtual void BuildRayTracingGeometry(CERayTracingGeometry* geometry,
		                                     CEVertexBuffer* vertexBuffer,
		                                     CEIndexBuffer* indexBuffer,
		                                     bool update) override final;
		virtual void BuildRayTracingScene(CERayTracingScene* scene,
		                                  const CERayTracingGeometryInstance* instances,
		                                  uint32 numInstances,
		                                  bool update) override final;
		virtual void GenerateMips(CETexture* texture) override final;
		virtual void TransitionTexture(CETexture* texture,
		                               CEResourceState beforeState,
		                               CEResourceState afterState) override final;
		virtual void TransitionBuffer(CEBuffer* buffer,
		                              CEResourceState beforeState,
		                              CEResourceState afterState) override final;
		virtual void UnorderedAccessTextureBarrier(CETexture* texture) override final;
		virtual void UnorderedAccessBufferBarrier(CEBuffer* buffer) override final;
		virtual void Draw(uint32 vertexCount, uint32 startVertexLocation) override final;
		virtual void
		DrawIndexed(uint32 indexCount, uint32 startIndexLocation, int32 baseVertexLocation) override final;
		virtual void DrawInstanced(uint32 vertexPerCountInstance, uint32 instanceCount, uint32 startVertexLocation,
		                           uint32 startInstanceLocation) override final;
		virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount,
		                                  uint32 startIndexLocation,
		                                  uint32 baseVertexLocation, uint32 startInstanceLocation) override final;
		virtual void
		Dispatch(uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ) override final;
		virtual void ClearState() override final;
		virtual void Flush() override final;
		virtual void BeginExternalCapture() override final;
		virtual void EndExternalCapture() override final;
	protected:
	private:
		void InternalClearState();

		void FlushGpu() {
			for (int i = 0; i < 3; i++) {
				uint64_t fenceValueForSignal = ++FenceValue;
				CommandQueue.GetQueue()->Signal(Fence.GetFence(), fenceValueForSignal);
				if (Fence.GetFence()->GetCompletedValue() < FenceValue) {
					Fence.GetFence()->SetEventOnCompletion(fenceValueForSignal, Fence.GetEvent());
					WaitForSingleObject(Fence.GetEvent(), INFINITE);
				}
			}
		}

	private:
		CEDXCommandListHandle CommandList;
		CEDXFenceHandle Fence;
		CEDXCommandQueueHandle CommandQueue;

		uint64 FenceValue = 0;
		uint64 NextCommandBatch = 0;

		CEArray<CEDXCommandBatch> CommandBatches;
		CEDXCommandBatch* CommandBatch = nullptr;

		CEArray<CERef<CEDXGPUProfiler>> ResolveProfilers;

		CERef<CEDXComputePipelineState> GenerateMipsTex2D_PSO;
		CERef<CEDXComputePipelineState> GenerateMipsTexCube_PSO;

		CERef<CEDXGraphicsPipelineState> CurrentGraphicsPipelineState;
		CERef<CEDXComputePipelineState> CurrentComputePipelineState;
		CERef<CEDXRootSignature> CurrentRootSignature;

		CEDXShaderConstantsCache ShaderConstantsCache;
		CEDXDescriptorCache DescriptorCache;
		CEDXResourceBarrierBatcher BarrierBatcher;

		bool IsReady = false;
		bool IsCapturing = false;
	};
}
