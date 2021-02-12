#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "CEDescriptorAllocation.h"

namespace ConceptFramework::GraphicsEngine::Direct3D {
	class CEDevice;
	class CEResource;
	namespace wrl = Microsoft::WRL;

	class CEShaderResourceView {
	public:
		std::shared_ptr<CEResource> GetResource() const {
			return m_resource;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const {
			return m_descriptor.GetDescriptorHandle();
		}

	protected:
		CEShaderResourceView(CEDevice& device, const std::shared_ptr<CEResource>& resource,
		                            const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr);
		virtual ~CEShaderResourceView() = default;

	private:
		CEDevice& m_device;
		std::shared_ptr<CEResource> m_resource;
		CEDescriptorAllocation m_descriptor;
	};
}
