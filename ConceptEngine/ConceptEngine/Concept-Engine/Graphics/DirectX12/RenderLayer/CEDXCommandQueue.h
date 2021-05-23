#pragma once
#include "CEDXDeviceElement.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXCommandQueueHandle : public CEDXDeviceElement {
	public:
		CEDXCommandQueueHandle(CEDXDevice* device) : CEDXDeviceElement(device), Queue(nullptr), Desc() {

		}

		bool Create(D3D12_COMMAND_LIST_TYPE type) {
			return true;
		}

	protected:
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> Queue;
		D3D12_COMMAND_QUEUE_DESC Desc;

	};
}
