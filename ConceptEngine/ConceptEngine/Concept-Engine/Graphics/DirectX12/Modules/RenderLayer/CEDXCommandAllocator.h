#pragma once
#include <string>
#include <d3d12.h>
#include <wrl/client.h>

#include "CEDXDeviceElement.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::RenderLayer {
	class CEDXCommandAllocatorHandle : public CEDXDeviceElement {
	public:
		CEDXCommandAllocatorHandle(CEDXDevice* device) : CEDXDeviceElement(device), Allocator(nullptr) {

		}

		bool Create(D3D12_COMMAND_LIST_TYPE type);
		bool Reset();
		void SetName(const std::string& name);

		ID3D12CommandAllocator* GetAllocator() const {
			return Allocator.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> Allocator;
	};
}
