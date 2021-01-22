#include "CEShaderResourceView.h"

#include <cassert>


#include "CEDevice.h"
#include "CEResource.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEShaderResourceView::CEShaderResourceView(CEDevice& device, const std::shared_ptr<CEResource>& resource,
                                           const D3D12_SHADER_RESOURCE_VIEW_DESC* srv): m_device(device),
	m_resource(resource) {
	assert(resource || srv);
	auto d3d12Resource = m_resource ? m_resource->GetD3D12Resource() : nullptr;
	auto d3d12Device = m_device.GetDevice();
	m_descriptor = m_device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	d3d12Device->CreateShaderResourceView(d3d12Resource.Get(), srv, m_descriptor.GetDescriptorHandle());
}
