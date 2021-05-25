#pragma once
#include "CEDXDeviceElement.h"
#include "CEDXFence.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXCommandQueueHandle : public CEDXDeviceElement {
	public:
		CEDXCommandQueueHandle(CEDXDevice* device) : CEDXDeviceElement(device), Queue(nullptr), Desc() {

		}

		bool Create(D3D12_COMMAND_LIST_TYPE type) {
			return true;
		}

		bool SignalFence(CEDXFenceHandle& fence, uint64 fenceValue) {
			HRESULT result = Queue->Signal(fence.GetFence(), fenceValue);
			if (result == DXGI_ERROR_DEVICE_REMOVED) {
				DeviceRemovedHandler(GetDevice());
			}

			return SUCCEEDED(result);
		}

		bool WaitForFence(CEDXFenceHandle& fence, uint64 fenceValue) {
			HRESULT result = Queue->Wait(fence.GetFence(), fenceValue);
			if (result == DXGI_ERROR_DEVICE_REMOVED) {
				DeviceRemovedHandler(GetDevice());
			}

			return SUCCEEDED(result);
		}

		void ExecuteCommandList(CEDXCommandListHandle* commandList) {
			ID3D12CommandList* commandLists[] = {commandList->GetCommandList()};
			Queue->ExecuteCommandLists(1, commandLists);
		}

		void SetName(const std::string& name) {
			std::wstring wName = ConvertToWString(name);
			Queue->SetName(wName.c_str());
		}

		ID3D12CommandQueue* GetQueue() const {
			return Queue.Get();
		}

		const D3D12_COMMAND_QUEUE_DESC& GetDesc() const {
			return Desc;
		}

		D3D12_COMMAND_LIST_TYPE GetType() const {
			return Desc.Type;
		}

	protected:
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> Queue;
		D3D12_COMMAND_QUEUE_DESC Desc;

	};
}
