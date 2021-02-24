#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "../Libraries/d3dx12.h"
#include "../../../Tools/CEUtils.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	//TODO: FIX!!!
	class CEDefaultBuffer {
	public:
		CEDefaultBuffer(ID3D12Device* device,
		                ID3D12GraphicsCommandList* commandList,
		                const void* initialData,
		                UINT64 byteSize,
		                Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer):
			m_elementByteSize(byteSize),
			m_mappedData(initialData) {

			auto resourceDescBuffer = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

			//Create actual default buffer resource
			auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			ThrowIfFailed(device->CreateCommittedResource(
				&defaultHeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDescBuffer,
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(m_defaultBuffer.GetAddressOf())
			));

			//In order to copy CPU memory data into out default buffer, we need to create an intermediate upload heap
			auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto uploadResourceDescBuffer = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
			ThrowIfFailed(device->CreateCommittedResource(&uploadHeapProperties,
			                                              D3D12_HEAP_FLAG_NONE,
			                                              &uploadResourceDescBuffer,
			                                              D3D12_RESOURCE_STATE_GENERIC_READ,
			                                              nullptr,
			                                              IID_PPV_ARGS(uploadBuffer.GetAddressOf())));
			//Describe data we want to copy into default buffer
			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = initialData;
			subResourceData.RowPitch = byteSize;
			subResourceData.SlicePitch = subResourceData.RowPitch;

			//Schedule to cpy data to default buffer resource. At high level helper function UploadSubresources will copy CPU memory
			//into intermediate upload heap. Then, using ID3D12CommandList::CopySubresourceRegion, intermediate upload heap data will be copied to mBuffer
			auto defaultBufferTransitionCopy = CD3DX12_RESOURCE_BARRIER::Transition(
				m_defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COMMON,
				D3D12_RESOURCE_STATE_COPY_DEST);
			commandList->ResourceBarrier(1, &defaultBufferTransitionCopy);
			UpdateSubresources<1>(
				commandList,
				m_defaultBuffer.Get(),
				uploadBuffer.Get(),
				0,
				0,
				1,
				&subResourceData
			);

			auto defaultBufferTransitionRead = CD3DX12_RESOURCE_BARRIER::Transition(
				m_defaultBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_GENERIC_READ);
			commandList->ResourceBarrier(1, &defaultBufferTransitionRead);

			//NOTE: Upload buffer has to be kept alive after above function calls because command list has not been executed yet that performs actual copy.
			//Caller can Release uploadBuffer after it knows copy has been executed
		};


		CEDefaultBuffer(const CEDefaultBuffer&) = delete;
		CEDefaultBuffer& operator=(const CEDefaultBuffer& rhs) = delete;


		~CEDefaultBuffer() {
			if (m_defaultBuffer != nullptr) {
				m_defaultBuffer->Unmap(0, nullptr);
			}

			m_mappedData = nullptr;
		}

		ID3D12Resource* Resource() const {
			return m_defaultBuffer.Get();
		}

	protected:
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_defaultBuffer;
		const void* m_mappedData = nullptr;

		UINT64 m_elementByteSize = 0;
	};
}
