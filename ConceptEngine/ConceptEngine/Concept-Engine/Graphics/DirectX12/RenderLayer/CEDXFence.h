#pragma once
#include "CEDXDeviceElement.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXFenceHandle : public CEDXDeviceElement {
	public:
		CEDXFenceHandle(CEDXDevice* device) : CEDXDeviceElement(device), Fence(nullptr), Event(0) {

		}

		~CEDXFenceHandle() {
			::CloseHandle(Event);
		}

		bool Create(uint64 initialValue) {
			HRESULT result = GetDevice()->GetDevice()->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE,
			                                                       IID_PPV_ARGS(&Fence));
			if (FAILED(result)) {
				CE_LOG_INFO("[CEDXFenceHandle]: Failed to create Fence");
				return false;
			}

			Event = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (Event == NULL) {
				CE_LOG_ERROR("[CEDXFenceHandle]: Failed to create Event for Fence");
				return false;
			}
			return true;
		}

		bool WaitForValue(const uint64 waitValue) {
			HRESULT result = Fence->SetEventOnCompletion(waitValue, Event);
			if (SUCCEEDED(result)) {
				::WaitForSingleObjectEx(Event, INFINITE, FALSE);
				return true;
			}
			else {
				return false;
			}
		}

		bool Signal(uint64 value) {
			HRESULT result = Fence->Signal(value);
			return SUCCEEDED(result);
		}

		void SetName(const std::string& name) {
			std::wstring wName = ConvertToWString(name);
			Fence->SetName(wName.c_str());
		}

		ID3D12Fence* GetFence() const {
			return Fence.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
		HANDLE Event = 0;
	};
}
