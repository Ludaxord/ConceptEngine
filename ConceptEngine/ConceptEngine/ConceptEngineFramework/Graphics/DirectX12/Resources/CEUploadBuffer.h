#pragma once
#include <d3d12.h>

#include "../../../Tools/CEUtils.h"
#include "../Libraries/d3dx12.h"

template <typename T>
class CEUploadBuffer {
public:
	//TODO: create ConstantBuffer class
	CEUploadBuffer(ID3D12Device* d3dDevice, UINT elementsCount, bool isConstantBuffer) :
		m_isConstantBuffer(isConstantBuffer) {
		m_elementByteSize = sizeof(T);

		/*Constant buffer elements need to be multiples of 256 bytes.
			* This is because hardware can only view constant data
			* at m*256 byte offset and of n*256 byte lengths.
			*
			* typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
			* UINT64 OffsetInBytes;		// Multiple of 256
			* UINT SizeInBytes;			// Multiple of 256
			* } D3D12_CONSTANT_BUFFER_VIEW_DESC
			* 
			* Constant buffers must be a multiple of minimum hardware allocation size (usually 256 bytes).
			* So round up to nearest multiple of 256. We do this by adding 256 and then masking off the lower 2 bytes which store all bits < 256.
			* Example: Suppose byte size = 300.
			* (300 + 255) & ~255
			* 555 & ~255
			* 0x022B & ~0x00ff
			* 0x022B &	0xff00
			* 0x0200
			* 512
			* 
		*/
		if (m_isConstantBuffer)
			m_elementByteSize = (sizeof(T) + 255) & ~255;

		auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceBuffer = CD3DX12_RESOURCE_DESC::Buffer(m_elementByteSize * elementsCount);
		ThrowIfFailed(d3dDevice->CreateCommittedResource(&uploadHeapProperties,
		                                                 D3D12_HEAP_FLAG_NONE,
		                                                 &resourceBuffer,
		                                                 D3D12_RESOURCE_STATE_GENERIC_READ,
		                                                 nullptr,
		                                                 IID_PPV_ARGS(&m_uploadBuffer)
		));

		ThrowIfFailed(m_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(m_mappedData)));

		//We do not need to unmap until we are done with resource.
		//However, we must not write to resource while it is in use by GPU (so we must use synchronization techniques)
	}

	CEUploadBuffer(const CEUploadBuffer&) = delete;
	CEUploadBuffer& operator=(const CEUploadBuffer& rhs) = delete;

	~CEUploadBuffer() {
		if (m_uploadBuffer != nullptr) {
			m_uploadBuffer->Unmap(0, nullptr);
		}

		m_mappedData = nullptr;
	}

	ID3D12Resource* Resource() const {
		return m_uploadBuffer.Get();
	}

	void CopyData(int elementIndex, const T& data) {
		memcpy(&m_mappedData[elementIndex * m_elementByteSize], &data, sizeof(T));
	}

protected:
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer;
	BYTE* m_mappedData = nullptr;

	bool m_isConstantBuffer = false;
	UINT m_elementByteSize = 0;
};
