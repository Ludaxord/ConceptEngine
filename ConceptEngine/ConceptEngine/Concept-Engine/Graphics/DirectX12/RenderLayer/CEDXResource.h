#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <string>

#include "CEDXDeviceElement.h"
#include "../../../Core/Common/CERefCountedObject.h"
#include "../../../Utilities/CEStringUtilities.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXResource : public CEDXDeviceElement, public CERefCountedObject {
	public:
		CEDXResource(CEDXDevice* device, const Microsoft::WRL::ComPtr<ID3D12Resource>& nativeResource);
		CEDXResource(CEDXDevice* device, const D3D12_RESOURCE_DESC& desc, D3D12_HEAP_TYPE heapType);
		~CEDXResource() = default;

		bool Create(D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* optimizedClearValue);

		void* Map(uint32 subResource, const D3D12_RANGE* range);
		void Unmap(uint32 subResource, const D3D12_RANGE* range);

		void SetName(const std::string& name) {
			std::wstring wName = ConvertToWString(name);
			DXResource->SetName(wName.c_str());
		}

		ID3D12Resource* GetResource() const {
			return DXResource.Get();
		}

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
			return Address;
		}

		D3D12_HEAP_TYPE GetHeapType() const {
			return HeapType;
		}

		D3D12_RESOURCE_DIMENSION GetDimension() const {
			return Desc.Dimension;
		}

		D3D12_RESOURCE_STATES GetState() const {
			return ResourceState;
		}

		const D3D12_RESOURCE_DESC& GetDesc() const {
			return Desc;
		}

		uint64 GetWidth() const {
			return Desc.Width;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> DXResource;
		D3D12_HEAP_TYPE HeapType;
		D3D12_RESOURCE_STATES ResourceState;
		D3D12_RESOURCE_DESC Desc;
		D3D12_GPU_VIRTUAL_ADDRESS Address = 0;
	};
}
