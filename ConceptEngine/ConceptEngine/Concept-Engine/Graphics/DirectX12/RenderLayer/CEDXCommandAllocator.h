#pragma once
#include <string>
#include <d3d12.h>
#include <wrl/client.h>

#include "CEDXDeviceElement.h"

#include "../../../Utilities/CEStringUtilities.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXCommandAllocatorHandle : public CEDXDeviceElement {
	public:
		CEDXCommandAllocatorHandle(CEDXDevice* device) : CEDXDeviceElement(device), Allocator(nullptr) {

		}

		bool Create(D3D12_COMMAND_LIST_TYPE type) {
			HRESULT result = GetDevice()->GetDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(&Allocator));
			if (SUCCEEDED(result)) {
				CE_LOG_INFO("[CEDXDevice]: Create Command Allocator");
				return true;
			}

			CE_LOG_ERROR("[CEDXDevice]: Failed to create Command Allocator");
			return false;
		};

		bool Reset() {
			HRESULT result = Allocator->Reset();
			if (result == DXGI_ERROR_DEVICE_REMOVED) {
				DeviceRemovedHandler(GetDevice());
			}

			return SUCCEEDED(result);
		};

		void SetName(const std::string& name) {
			std::wstring wName = ConvertToWString(name);
			Allocator->SetName(wName.c_str());
		};

		ID3D12CommandAllocator* GetAllocator() const {
			return Allocator.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> Allocator;
	};
}
