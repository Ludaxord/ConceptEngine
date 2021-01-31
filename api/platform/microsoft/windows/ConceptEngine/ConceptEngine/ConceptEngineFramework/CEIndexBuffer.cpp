#include "CEIndexBuffer.h"

#include <cassert>


#include "d3dx12.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEIndexBuffer::CEIndexBuffer(CEDevice& device, size_t numIndicies, DXGI_FORMAT indexFormat)
	: CEBuffer(device, CD3DX12_RESOURCE_DESC::Buffer(numIndicies * (indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4)))
	  , m_numIndices(numIndicies)
	  , m_indexFormat(indexFormat)
	  , m_indexBufferView{} {
	assert(indexFormat == DXGI_FORMAT_R16_UINT || indexFormat == DXGI_FORMAT_R32_UINT);
	CreateIndexBufferView();
}

CEIndexBuffer::CEIndexBuffer(CEDevice& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource,
                         size_t numIndicies, DXGI_FORMAT indexFormat)
	: CEBuffer(device, resource)
	  , m_numIndices(numIndicies)
	  , m_indexFormat(indexFormat)
	  , m_indexBufferView{} {
	assert(indexFormat == DXGI_FORMAT_R16_UINT || indexFormat == DXGI_FORMAT_R32_UINT);
	CreateIndexBufferView();
}

void CEIndexBuffer::CreateIndexBufferView() {
	UINT bufferSize = m_numIndices * (m_indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4);

	m_indexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes = bufferSize;
	m_indexBufferView.Format = m_indexFormat;
}
