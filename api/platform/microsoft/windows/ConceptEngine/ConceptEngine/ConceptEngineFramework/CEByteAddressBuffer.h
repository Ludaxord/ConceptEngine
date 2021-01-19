#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "CEBuffer.h"

namespace ConceptEngine::GraphicsEngine::Direct3D {
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	class CEByteAddressBuffer : public CEBuffer {
	public:
		size_t GetBufferSize() const {
			return m_bufferSize;
		}

	protected:
		CEByteAddressBuffer(CEDevice& device, const D3D12_RESOURCE_DESC& resDesc);
		CEByteAddressBuffer(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource);
		virtual ~CEByteAddressBuffer() = default;

	private:
		size_t m_bufferSize;
	};
}
