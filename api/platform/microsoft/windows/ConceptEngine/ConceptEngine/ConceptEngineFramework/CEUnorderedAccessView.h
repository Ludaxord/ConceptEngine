#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "CEDescriptorAllocation.h"

namespace ConceptFramework::GraphicsEngine::Direct3D {
	namespace wrl = Microsoft::WRL;

	class CEDevice;
	class CEResource;

	class CEUnorderedAccessView {
	public:
		std::shared_ptr<CEResource> GetResource() const {
			return m_resource;
		}

		std::shared_ptr<CEResource> GetCounterResource() const {
			return m_counterResource;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const {
			return m_descriptor.GetDescriptorHandle();
		}

	protected:
		CEUnorderedAccessView(CEDevice& device,
		                      const std::shared_ptr<CEResource>& resource,
		                      const std::shared_ptr<CEResource>& counterResource = nullptr,
		                      const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav = nullptr);
		virtual ~CEUnorderedAccessView() = default;

	private:
		CEDevice& m_device;
		std::shared_ptr<CEResource> m_resource;
		std::shared_ptr<CEResource> m_counterResource;
		CEDescriptorAllocation m_descriptor;
	};
}
