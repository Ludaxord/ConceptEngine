#include "CEResource.h"

#include "CEDevice.h"
#include "CEHelper.h"
#include "CEResourceStateTracker.h"
#include "d3dx12.h"

using namespace Concept::GraphicsEngine::Direct3D;


void CEResource::SetName(const std::wstring& name) {
	m_resourceName = name;
	if (m_resource && !m_resourceName.empty()) {
		m_resource->SetName(m_resourceName.c_str());
	}
}

bool CEResource::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const {
	return (m_formatSupport.Support1 & formatSupport) != 0;
}

bool CEResource::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const {
	return (m_formatSupport.Support2 & formatSupport) != 0;
}

CEResource::CEResource(CEDevice& device, const D3D12_RESOURCE_DESC& resourceDesc,
                       const D3D12_CLEAR_VALUE* clearValue): m_device(device) {
	auto d3d12Device = m_device.GetDevice();
	if (clearValue) {
		m_clearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
	}

	ThrowIfFailed(d3d12Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	                                                   D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON,
	                                                   m_clearValue.get(), IID_PPV_ARGS(&m_resource)));
	CEResourceStateTracker::AddGlobalResourceState(m_resource.Get(), D3D12_RESOURCE_STATE_COMMON);
	CheckFeatureSupport();
}

CEResource::CEResource(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource,
                       const D3D12_CLEAR_VALUE* clearValue): m_device(device), m_resource(resource) {
	if (clearValue) {
		m_clearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
	}
	CheckFeatureSupport();
}

void CEResource::CheckFeatureSupport() {
	auto d3d12Device = m_device.GetDevice();

	auto desc = m_resource->GetDesc();
	ThrowIfFailed(d3d12Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &m_formatSupport,
	                                               sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
}
