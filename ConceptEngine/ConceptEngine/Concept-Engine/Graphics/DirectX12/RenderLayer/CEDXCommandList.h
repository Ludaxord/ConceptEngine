#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "CEDXCommandAllocator.h"
#include "CEDXDevice.h"
#include "CEDXDeviceElement.h"
#include "CEDXResource.h"
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

		void ResourceBarrier(struct D3D12_RESOURCE_BARRIER* barriers, uint32 numBarriers) {
			CommandList->ResourceBarrier(numBarriers, barriers);
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

		//TODO: Finish mapping all functions of Command List
	
	protected:
	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList5;
		bool IsReady = false;
	};
}
