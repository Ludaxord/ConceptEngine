#pragma once

#include <d3d12.h>

#include "CEDXCommandList.h"
#include "CEDXCommandQueue.h"
#include "CEDXDescriptorHeap.h"
#include "CEDXDevice.h"
#include "CEDXDeviceElement.h"
#include "CEDXFence.h"
#include "CEDXResource.h"
#include "../../../Main/RenderLayer/CEICommandContext.h"
#include "../../../../Math/CEMath.h"
#include "../../../../Core/Containers/CEArray.h"
#include "../../../../Memory/CEMemory.h"

namespace ConceptEngine::Graphics {

	using namespace Main::RenderLayer;

	namespace DirectX12::Modules::RenderLayer {
		struct CEDXUploadAllocation {

		};

		struct CEDXGPUResourceUploader {

		};

		struct CEDXCommandBatch {
		public:
			CEDXCommandBatch(CEDXDevice* device);
			~CEDXCommandBatch() = default;

			bool Create();

			bool Reset() {
				return false;
			}

			void AddInUseResource(CEResource* resource) {
				if (resource) {
					Resources.EmplaceBack(Core::Common::MakeSharedRef<CEResource>(resource));
				}
			}

			void AddInUseResource(CEDXResource* resource) {
				if (resource) {
					DXResources.EmplaceBack(Core::Common::MakeSharedRef<CEDXResource>(resource));
				}
			}

			void AddInUseResource(const Microsoft::WRL::ComPtr<CEDXResource>* resource) {
				if (resource) {
					NativeResources.EmplaceBack(resource);
				}
			}


		protected:
		private:
			CEDXDevice* Device = nullptr;

			CEDXCommandAllocatorHandle CommandAllocator;
			CEDXGPUResourceUploader GPUResourceUploader;

			Core::Common::CERef<CEDXOnlineDescriptorHeap> OnlineResourceDescriptorHeap;
			Core::Common::CERef<CEDXOnlineDescriptorHeap> OnlineSamplerDescriptorHeap;

			Core::Common::CERef<CEDXOnlineDescriptorHeap> OnlineRayTracingResourceDescriptorHeap;
			Core::Common::CERef<CEDXOnlineDescriptorHeap> OnlineRayTracingSamplerDescriptorHeap;

			Core::Containers::CEArray<Core::Common::CERef<CEDXResource>> DXResources;
			Core::Containers::CEArray<Core::Common::CERef<CEResource>> Resources;
			Core::Containers::CEArray<Microsoft::WRL::ComPtr<ID3D12Resource>> NativeResources;
		};

		struct CEDXResourceBarrierBatcher {
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
				}
			}

			const D3D12_RESOURCE_BARRIER* GetBarriers() const {
				return Barriers.Data();
			}

			uint32 GetNumBarriers() const {
				return Barriers.Size();
			}

		private:
			Core::Containers::CEArray<D3D12_RESOURCE_BARRIER> Barriers;
		};


		class CEDXCommandContext : public CEICommandContext, public CEDXDeviceElement {
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
			virtual void BeginRenderPass() override final;
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
			SetGraphicsPipelineState(CEGraphicsPipelineState* pipelineState) override final;
			virtual void
			SetComputePipelineState(CEComputePipelineState* pipelineState) override final;
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
			                             CESamplerState* samplerState,
			                             uint32 parameterIndex) override final;
			virtual void SetSamplerStates(CEShader* shader,
			                              CESamplerState* const* samplerStates,
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

			CEDXCommandListHandle CommandList;
			CEDXFenceHandle Fence;
			CEDXCommandQueueHandle CommandQueue;

			uint64 FenceValue = 0;
			uint64 NextCommandBatch = 0;

			Core::Containers::CEArray<CEDXCommandBatch> CommandBatches;
			CEDXCommandBatch* CommandBatch = nullptr;

			Core::Containers::CEArray<Core::Common::CERef<CEDXGPUProfiler>> ResolveProfilers;

			Core::Common::CERef<CEDXComputePipelineState> GenerateMipsTex2D_PSO;
			Core::Common::CERef<CEDXComputePipelineState> GenerateMipsTexCube_PSO;

			Core::Common::CERef<CEDXGraphicsPipelineState> CurrentGraphicsPipelineState;
			Core::Common::CERef<CEDXComputePipelineState> CurrentComputePipelineState;
			Core::Common::CERef<CEDXRootSignature> CurrentRootSignature;

			CEDXShaderConstantsCache ShaderConstantsCache;
			CEDXDescriptorCache DescriptorCache;
			CEDXResourceBarrierBatcher BarrierBatcher;

			bool IsReady = false;
			bool IsCapturing = false;
		};
	}}
