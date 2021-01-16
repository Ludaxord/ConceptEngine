#pragma once
#include "CED3D12Utils.h"
#include "CEHelper.h"
#include "d3dx12.h"

#include <wrl.h>

namespace wrl = Microsoft::WRL;

template <typename T>
class CED3D12UploadBuffer {
public:
	CED3D12UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) : m_isConstantBuffer(
		isConstantBuffer) {
		m_bytesElementSize = GetBufferByteSize(sizeof(T), m_isConstantBuffer);
		ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		                                              D3D12_HEAP_FLAG_NONE,
		                                              &CD3DX12_RESOURCE_DESC::Buffer(m_bytesElementSize * elementCount),
		                                              D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		                                              IID_PPV_ARGS(&m_UploadBuffer)));
		ThrowIfFailed(m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData)));
	}

	CED3D12UploadBuffer(const CED3D12UploadBuffer& rhs) = delete;
	CED3D12UploadBuffer& operator=(const CED3D12UploadBuffer& rhs) = delete;
	~CED3D12UploadBuffer() {
		if (m_UploadBuffer != nullptr)
			m_UploadBuffer->Unmap(0, nullptr);
		m_MappedData = nullptr;
	}

	ID3D12Resource* Buffer() const {
		return m_UploadBuffer.Get();
	}

	void CopyData(int elementIndex, const T& data) {
		memcpy(&m_MappedData[elementIndex * m_bytesElementSize], &data, sizeof(T));
	}

private:
	UINT GetBufferByteSize(UINT size, bool isConstantBuffer) {
		UINT elementBytesSize = 0;
		if (isConstantBuffer) {
			elementBytesSize = CED3D12Utils::CalcConstantBufferSize(size);
		}
		else {
			elementBytesSize = sizeof(T);
		}
		return elementBytesSize;
	}

	wrl::ComPtr<ID3D12Resource> m_UploadBuffer;
	BYTE* m_MappedData = nullptr;

	UINT m_bytesElementSize = 0;
	bool m_isConstantBuffer = false;
};
