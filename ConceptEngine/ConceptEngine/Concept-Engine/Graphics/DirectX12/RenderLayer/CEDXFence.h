#pragma once
#include "CEDXDeviceElement.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXFenceHandle : public CEDXDeviceElement {
	public:
		CEDXFenceHandle(CEDXDevice* device) : CEDXDeviceElement(device), Fence(nullptr), Event(0) {

		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
		HANDLE Event = 0;
	};
}
