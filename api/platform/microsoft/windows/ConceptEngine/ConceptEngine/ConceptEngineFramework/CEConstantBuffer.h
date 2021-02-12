#pragma once

#include <wrl.h>

#include "CEBuffer.h"

namespace ConceptFramework::GraphicsEngine::Direct3D {
	namespace wrl = Microsoft::WRL;

	class CEConstantBuffer : public CEBuffer {
	public:
		size_t GetSizeInBytes() const {
			return m_sizeInBytes;
		}

	protected:
		CEConstantBuffer(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource);
		virtual ~CEConstantBuffer();
	private:
		size_t m_sizeInBytes;
	};
}
