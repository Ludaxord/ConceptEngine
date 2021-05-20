#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "CEDXCommandAllocator.h"
#include "CEDXDevice.h"
#include "CEDXDeviceElement.h"
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

		void ResourceBarrier(struct D3D12_RESOURCE_BARRIER* barriers, uint32 numBarriers) {
			CommandList->ResourceBarrier(numBarriers, barriers);
		}

	protected:
	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList5;
		bool IsReady = false;
	};
}
