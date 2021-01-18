#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "CEDirectXBuffer.h"

namespace ConceptEngine::GraphicsEngine::DirectX {
	class CEDirectXDevice;
	namespace wrl = Microsoft::WRL;

	class CEDirectXByteAddressBuffer : public CEDirectXBuffer {
	public:
		size_t GetBufferSize() const {
			return m_bufferSize;
		}

	protected:
		CEDirectXByteAddressBuffer(CEDirectXDevice& device, const D3D12_RESOURCE_DESC& resDesc);
		CEDirectXByteAddressBuffer(CEDirectXDevice& device, wrl::ComPtr<ID3D12Resource> resource);
		virtual ~CEDirectXByteAddressBuffer() = default;

	private:
		size_t m_bufferSize;
	};
}
