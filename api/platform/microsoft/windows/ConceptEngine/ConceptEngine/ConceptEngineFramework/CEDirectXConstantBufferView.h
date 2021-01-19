#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "CEDirectXDescriptorAllocation.h"

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	class CEDirectXDevice;
	class CEDirectXConstantBuffer;
	namespace wrl = Microsoft::WRL;

	class CEDirectXConstantBufferView {

	public:
		std::shared_ptr<CEDirectXConstantBuffer> GetConstantBuffer() const {
			return m_constantBuffer;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() {
			return m_descriptor.GetDescriptorHandle();
		}

	protected:
		CEDirectXConstantBufferView(CEDirectXDevice& device,
		                            const std::shared_ptr<CEDirectXConstantBuffer>& constantBuffer, size_t offset = 0);
		virtual ~CEDirectXConstantBufferView() = default;

	private:
		CEDirectXDevice& m_device;
		std::shared_ptr<CEDirectXConstantBuffer> m_constantBuffer;
		CEDirectXDescriptorAllocation m_descriptor;
	};
}
