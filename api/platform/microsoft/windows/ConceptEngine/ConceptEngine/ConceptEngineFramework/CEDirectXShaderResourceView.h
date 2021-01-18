#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "CEDirectXDescriptorAllocation.h"

namespace ConceptEngine::GraphicsEngine::DirectX {
	class CEDirectXDevice;
	class CEDirectXResource;
	namespace wrl = Microsoft::WRL;

	class CEDirectXShaderResourceView {
	public:
		std::shared_ptr<CEDirectXResource> GetResource() const {
			return m_resource;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const {
			return m_descriptor.GetDescriptorHandle();
		}

	protected:
		CEDirectXShaderResourceView(CEDirectXDevice& device, const std::shared_ptr<CEDirectXResource>& resource,
		                            const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr);
		virtual ~CEDirectXShaderResourceView() = default;

	private:
		CEDirectXDevice& m_device;
		std::shared_ptr<CEDirectXResource> m_resource;
		CEDirectXDescriptorAllocation m_descriptor;
	};
}
