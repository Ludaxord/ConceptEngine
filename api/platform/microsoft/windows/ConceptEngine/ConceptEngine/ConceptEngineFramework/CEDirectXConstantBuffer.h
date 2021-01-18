#pragma once

#include <wrl.h>

#include "CEDirectXBuffer.h"

namespace ConceptEngine::GraphicsEngine::DirectX {
	namespace wrl = Microsoft::WRL;

	class CEDirectXConstantBuffer : public CEDirectXBuffer {
	public:
		size_t GetSizeInBytes() const {
			return m_sizeInBytes;
		}

	protected:
		CEDirectXConstantBuffer(CEDirectXDevice& device, wrl::ComPtr<ID3D12Resource> resource);
		virtual ~CEDirectXConstantBuffer();
	private:
		size_t m_sizeInBytes;
	};
}
