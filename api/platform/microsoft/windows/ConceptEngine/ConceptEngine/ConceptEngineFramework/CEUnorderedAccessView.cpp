#include "CEUnorderedAccessView.h"

#include <cassert>

#include "CEDevice.h"
#include "CEResource.h"
using namespace ConceptFramework::GraphicsEngine::Direct3D;

CEUnorderedAccessView::CEUnorderedAccessView(CEDevice& device, const std::shared_ptr<CEResource>& resource,
                                             const std::shared_ptr<CEResource>& counterResource,
                                             const D3D12_UNORDERED_ACCESS_VIEW_DESC* uav)
	: m_device(device)
	  , m_resource(resource)
	  , m_counterResource(counterResource) {
	assert(m_resource || uav);

	auto d3d12Device = m_device.GetDevice();
	auto d3d12Resource = m_resource ? m_resource->GetD3D12Resource() : nullptr;
	auto d3d12CounterResource = m_counterResource ? m_counterResource->GetD3D12Resource() : nullptr;

	if (m_resource) {
		auto d3d12ResourceDesc = m_resource->GetD3D12ResourceDesc();

		// Resource must be created with the D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS flag.
		assert((d3d12ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0);
	}

	m_descriptor = m_device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	d3d12Device->CreateUnorderedAccessView(d3d12Resource.Get(), d3d12CounterResource.Get(), uav,
	                                       m_descriptor.GetDescriptorHandle());
}
