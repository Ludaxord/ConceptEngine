#include "CEConstantBufferView.h"

#include <cassert>

#include "CEConstantBuffer.h"
#include "CEDevice.h"
#include "CEHelper.h"

using namespace Concept::GraphicsEngine::Direct3D;

CEConstantBufferView::CEConstantBufferView(CEDevice& device, const std::shared_ptr<CEConstantBuffer>& constantBuffer,
                                           size_t offset): m_device(device), m_constantBuffer(constantBuffer) {
	assert(constantBuffer);

	auto d3d12Device = m_device.GetDevice();
	auto d3d12Resource = m_constantBuffer->GetD3D12Resource();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv;
	cbv.BufferLocation = d3d12Resource->GetGPUVirtualAddress() + offset;
	/*
	 * Constant buffer must be aligned for hardware requirements.
	 */
	cbv.SizeInBytes = Math::AlignUp(m_constantBuffer->GetSizeInBytes(),
	                                D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	m_descriptor = device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	d3d12Device->CreateConstantBufferView(&cbv, m_descriptor.GetDescriptorHandle());
}
