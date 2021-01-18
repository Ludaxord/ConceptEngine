#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "CEDirectXDescriptorAllocation.h"

namespace ConceptEngine::GraphicsEngine::DirectX {
	class CEDirectXDevice;
	class CEDirectXResource;
	namespace wrl = Microsoft::WRL;

	class CEDirectXUnorderedAccessView {
	public:
		std::shared_ptr<CEDirectXResource> GetResource() const {
			return m_resource;
		}

		std::shared_ptr<CEDirectXResource> GetCounterResource() const {
			return m_counterResource;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const {
			return m_descriptor.GetDescriptorHandle();
		}

	protected:
		CEDirectXUnorderedAccessView(CEDirectXDevice& device,
		                             const std::shared_ptr<CEDirectXResource>& resource,
		                             const std::shared_ptr<CEDirectXResource>& counterResource = nullptr,
		                             const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav = nullptr);
		virtual ~CEDirectXUnorderedAccessView() = default;

	private:
		CEDirectXDevice& m_device;
		std::shared_ptr<CEDirectXResource> m_resource;
		std::shared_ptr<CEDirectXResource> m_counterResource;
		CEDirectXDescriptorAllocation m_descriptor;
	};
}
