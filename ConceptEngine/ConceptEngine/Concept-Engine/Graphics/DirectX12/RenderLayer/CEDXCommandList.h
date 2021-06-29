#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "CEDXCommandAllocator.h"
#include "CEDXDevice.h"
#include "CEDXDeviceElement.h"
#include "CEDXResource.h"
#include "CEDXRootSignature.h"
#include "CEDXViews.h"
#include "../../../Core/Common/CETypes.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXCommandListHandle : public CEDXDeviceElement {
	public:
		CEDXCommandListHandle(CEDXDevice* device): CEDXDeviceElement(device), CommandList(nullptr),
		                                           CommandList5(nullptr) {

		}

		bool Create(D3D12_COMMAND_LIST_TYPE type, CEDXCommandAllocatorHandle& allocator,
		            ID3D12PipelineState* pipelineState) {
			HRESULT hr = GetDevice()->GetDevice()->CreateCommandList(1, type, allocator.GetAllocator(), pipelineState,
			                                                         IID_PPV_ARGS(&CommandList));
			if (SUCCEEDED(hr)) {
				CommandList->Close();

				CE_LOG_INFO("[CEDXDevice]: Created Command List");

				if (FAILED(CommandList.As<ID3D12GraphicsCommandList5>(&CommandList5))) {
					CE_LOG_ERROR("[CEDXCommandList]: Failed to Retrive DXR Command List");
					return false;
				}

				return true;
			}

			CE_LOG_ERROR("[CEDXCommandList]: Failed to create Command List");

			return false;
		}

		bool Reset(CEDXCommandAllocatorHandle& allocator) {
			IsReady = true;
			HRESULT result = CommandList->Reset(allocator.GetAllocator(), nullptr);
			if (result == DXGI_ERROR_DEVICE_REMOVED) {
				DeviceRemovedHandler(GetDevice());
			}
			
			return SUCCEEDED(result);
		}

		bool Close() {
			IsReady = false;

			HRESULT result = CommandList->Close();
			if (result == DXGI_ERROR_DEVICE_REMOVED) {
				DeviceRemovedHandler(GetDevice());
			}

			return SUCCEEDED(result);
		}

		void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, const float color[4], uint32 numRect,
		                           const D3D12_RECT* rects) {
			CommandList->ClearRenderTargetView(renderTargetView, color, numRect, rects);
		}

		void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView, D3D12_CLEAR_FLAGS flags, float depth,
		                           const uint8 stencil) {
			CommandList->ClearDepthStencilView(depthStencilView, flags, depth, stencil, 0, nullptr);
		}

		void ClearUnorderedAccessViewFloat(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, const CEDXUnorderedAccessView* view,
		                                   const float clearColor[4]) {
			const CEDXResource* resource = view->GetResource();
			CommandList->ClearUnorderedAccessViewFloat(gpuHandle, view->GetOfflineHandle(), resource->GetResource(),
			                                           clearColor, 0, nullptr);
		}

		void CopyBufferRegion(CEDXResource* destination, uint64 destinationOffset, CEDXResource* source,
		                      uint64 sourceOffset, uint64 sizeInBytes) {
			CopyBufferRegion(destination->GetResource(), destinationOffset, source->GetResource(), sourceOffset,
			                 sizeInBytes);
		}

		void CopyBufferRegion(ID3D12Resource* destination, uint64 destinationOffset, ID3D12Resource* source,
		                      uint64 sourceOffset, uint64 sizeInBytes) {
			CommandList->CopyBufferRegion(destination, destinationOffset, source, sourceOffset, sizeInBytes);
		};

		void CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION* destination, uint32 x, uint32 y, uint32 z,
		                       const D3D12_TEXTURE_COPY_LOCATION* source, const D3D12_BOX* sourceBox) {
			CommandList->CopyTextureRegion(destination, x, y, z, source, sourceBox);
		}

		void CopyResource(CEDXResource* destination, CEDXResource* source) {
			CopyResource(destination->GetResource(), source->GetResource());
		}

		void CopyResource(ID3D12Resource* destination, ID3D12Resource* source) {
			CommandList->CopyResource(destination, source);
		}

		void ResolveSubresource(CEDXResource* destination, CEDXResource* source, DXGI_FORMAT format) {
			CommandList->ResolveSubresource(destination->GetResource(), 0, source->GetResource(), 0, format);
		}

		void BuildRayTracingAccelerationStructure(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC* desc) {
			CommandList5->BuildRaytracingAccelerationStructure(desc, 0, nullptr);
		}

		void DispatchRays(const D3D12_DISPATCH_RAYS_DESC* desc) {
			CommandList5->DispatchRays(desc);
		}

		void Dispatch(uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ) {
			CommandList->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
		}

		void DrawInstanced(uint32 vertexCountPerInstance,
		                   uint32 instanceCount,
		                   uint32 startVertexLocation,
		                   uint32 startInstanceLocation) {
			CommandList->DrawInstanced(vertexCountPerInstance,
			                           instanceCount,
			                           startVertexLocation,
			                           startInstanceLocation);
		}

		void DrawIndexedInstanced(uint32 indexCountPerInstance,
		                          uint32 instanceCount,
		                          uint32 startIndexLocation,
		                          uint32 baseVertexLocation,
		                          uint32 startInstanceLocation) {
			CommandList->DrawIndexedInstanced(indexCountPerInstance,
			                                  instanceCount,
			                                  startIndexLocation,
			                                  baseVertexLocation,
			                                  startInstanceLocation);
		}

		void SetDescriptorHeaps(ID3D12DescriptorHeap* const* descriptorHeaps, uint32 descriptorHeapCount) {
			CommandList->SetDescriptorHeaps(descriptorHeapCount, descriptorHeaps);
		}

		void SetStateObject(ID3D12StateObject* stateObject) {
			CommandList5->SetPipelineState1(stateObject);
		}

		void SetPipelineState(ID3D12PipelineState* pipelineState) {
			CommandList->SetPipelineState(pipelineState);
		}

		void SetComputeRootSignature(CEDXRootSignature* rootSignature) {
			CommandList->SetComputeRootSignature(rootSignature->GetRootSignature());
		}

		void SetGraphicsRootSignature(CEDXRootSignature* rootSignature) {
			CommandList->SetGraphicsRootSignature(rootSignature->GetRootSignature());
		}

		void SetComputeRootDescriptorTable(uint32 rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor) {
			CommandList->SetComputeRootDescriptorTable(rootParameterIndex, baseDescriptor);
		}

		void SetGraphicsRootDescriptorTable(uint32 rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor) {
			CommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, baseDescriptor);
		}

		void SetComputeRoot32BitConstants(const void* sourceData,
		                                  uint32 num32BitValues,
		                                  uint32 destOffsetIn32BitValues,
		                                  uint32 rootParameterIndex) {
			CommandList->SetComputeRoot32BitConstants(rootParameterIndex, num32BitValues, sourceData,
			                                          destOffsetIn32BitValues);
		}

		void SetGraphicsRoot32BitConstants(const void* sourceData,
		                                   uint32 num32BitValues,
		                                   uint32 destOffsetIn32BitValues,
		                                   uint32 rootParameterIndex) {
			CommandList->SetGraphicsRoot32BitConstants(rootParameterIndex, num32BitValues, sourceData,
			                                           destOffsetIn32BitValues);
		}

		void SetVertexBuffers(uint32 start, const D3D12_VERTEX_BUFFER_VIEW* vertexBufferViews,
		                      uint32 vertexBufferViewCount) {
			CommandList->IASetVertexBuffers(start, vertexBufferViewCount, vertexBufferViews);
		}

		void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* indexBufferView) {
			CommandList->IASetIndexBuffer(indexBufferView);
		}

		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) {
			CommandList->IASetPrimitiveTopology(primitiveTopology);
		}

		void SetViewports(const D3D12_VIEWPORT* viewports, uint32 viewportCount) {
			CommandList->RSSetViewports(viewportCount, viewports);
		}

		void SetScissorRects(const D3D12_RECT* scissorRects, uint32 scissorRectCount) {
			CommandList->RSSetScissorRects(scissorRectCount, scissorRects);
		}

		void SetShadingRate(D3D12_SHADING_RATE baseShadingRate, const D3D12_SHADING_RATE_COMBINER* combiners) {
			CommandList5->RSSetShadingRate(baseShadingRate, combiners);
		}

		void SetShadingRateImage(ID3D12Resource* shadingRateImage) {
			CommandList5->RSSetShadingRateImage(shadingRateImage);
		}

		void SetBlendFactor(const float blendFactor[4]) {
			CommandList->OMSetBlendFactor(blendFactor);
		}

		void SetRenderTargets(const D3D12_CPU_DESCRIPTOR_HANDLE* renderTargetDescriptors,
		                      uint32 numRenderTargetsDescriptors, bool rtSingleHandletoDescriptorRange,
		                      const D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilDescriptor) {
			CommandList->OMSetRenderTargets(numRenderTargetsDescriptors, renderTargetDescriptors,
			                                rtSingleHandletoDescriptorRange, depthStencilDescriptor);
		}

		void ResourceBarrier(const D3D12_RESOURCE_BARRIER* barriers, uint32 numBarriers) {
			CommandList->ResourceBarrier(numBarriers, barriers);
		}

		void TransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState,
		                       D3D12_RESOURCE_STATES afterState, UINT subResource) {
			D3D12_RESOURCE_BARRIER barrier;
			Memory::CEMemory::Memzero(&barrier);

			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = resource;
			barrier.Transition.StateAfter = afterState;
			barrier.Transition.StateBefore = beforeState;
			barrier.Transition.Subresource = subResource;

			CommandList->ResourceBarrier(1, &barrier);
		}

		void UnorderedAccessBarrier(ID3D12Resource* resource) {
			D3D12_RESOURCE_BARRIER barrier;
			Memory::CEMemory::Memzero(&barrier);

			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barrier.UAV.pResource = resource;

			CommandList->ResourceBarrier(1, &barrier);
		}

		//TODO: for now keep it in one function but create renderpass class for it in future
		void BeginRenderPass(D3D12_RENDER_PASS_RENDER_TARGET_DESC renderPassRenderTargetDesc,
		                     D3D12_RENDER_PASS_DEPTH_STENCIL_DESC renderPassDepthStencilDesc,
		                     uint32 numRenderTargets) {
			if (!RenderPassRunning) {
				CommandList5->BeginRenderPass(numRenderTargets,
				                              &renderPassRenderTargetDesc,
				                              &renderPassDepthStencilDesc,
				                              D3D12_RENDER_PASS_FLAG_NONE);
				RenderPassRunning = true;
			}
		}

		void EndRenderPass() {
			if (RenderPassRunning) {
				CommandList5->EndRenderPass();
				RenderPassRunning = false;
			}
		}

		bool IsRecording() const {
			return IsReady;
		}

		void SetName(const std::string& name) {
			std::wstring wName = ConvertToWString(name);
			CommandList->SetName(wName.c_str());
		}

		ID3D12CommandList* GetCommandList() const {
			return CommandList.Get();
		}

		ID3D12GraphicsCommandList* GetGraphicsCommandList() const {
			return CommandList.Get();
		}

		ID3D12GraphicsCommandList5* GetDXRCommandList() const {
			return CommandList5.Get();
		}

	protected:
	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> CommandList5;
		bool IsReady = false;

		//TODO: keep flag to active render pass here for now
		bool RenderPassRunning = false;
	};
}
