#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "CEDescriptorAllocation.h"


namespace Concept::GraphicsEngine::Direct3D {
	class CEDevice;
	class CEConstantBuffer;
	namespace wrl = Microsoft::WRL;

	class CEConstantBufferView {

	public:
		std::shared_ptr<CEConstantBuffer> GetConstantBuffer() const {
			return m_constantBuffer;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() {
			return m_descriptor.GetDescriptorHandle();
		}

	protected:
		CEConstantBufferView(CEDevice& device,
		                            const std::shared_ptr<CEConstantBuffer>& constantBuffer, size_t offset = 0);
		virtual ~CEConstantBufferView() = default;

	private:
		CEDevice& m_device;
		std::shared_ptr<CEConstantBuffer> m_constantBuffer;
		CEDescriptorAllocation m_descriptor;
	};
}
