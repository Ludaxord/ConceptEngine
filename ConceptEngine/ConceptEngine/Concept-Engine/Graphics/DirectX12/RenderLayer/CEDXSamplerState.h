#pragma once
#include "CEDXDescriptorHeap.h"
#include "CEDXDeviceElement.h"
#include "../../Main/RenderLayer/CESamplerState.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXSamplerState : public CESamplerState, public CEDXDeviceElement {
	public:
		CEDXSamplerState(CEDXDevice* device, CEDXOfflineDescriptorHeap* offlineHeap) : CESamplerState(),
			CEDXDeviceElement(device), OfflineHeap(offlineHeap), OfflineHandle({0}), Desc() {
			Assert(offlineHeap != nullptr);
		}

		~CEDXSamplerState() {
			OfflineHeap->Free(OfflineHandle, OfflineHeapIndex);
		}

		bool Create(const D3D12_SAMPLER_DESC& desc) {
			OfflineHandle = OfflineHeap->Allocate(OfflineHeapIndex);
			if (OfflineHandle != 0) {
				Desc = desc;
				GetDevice()->CreateSampler(&Desc, OfflineHandle);
				return true;
			}

			return false;
		}

		virtual void* GetNativeResource() const override final {
			return reinterpret_cast<void*>(OfflineHandle.ptr);
		}

		virtual bool IsValid() const override {
			return OfflineHandle != 0;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetOfflineHandle() const {
			return OfflineHandle;
		}

		const D3D12_SAMPLER_DESC& GetDesc() const {
			return Desc;
		}

	protected:
	private:
		CEDXOfflineDescriptorHeap* OfflineHeap = nullptr;
		uint32 OfflineHeapIndex = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE OfflineHandle;
		D3D12_SAMPLER_DESC Desc;
	};
}
