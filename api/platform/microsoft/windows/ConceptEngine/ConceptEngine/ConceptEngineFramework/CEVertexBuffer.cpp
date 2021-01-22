#include "CEVertexBuffer.h"

#include "d3dx12.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEVertexBuffer::CEVertexBuffer(CEDevice& device, size_t numVertices, size_t vertexStride): CEBuffer(
		device, CD3DX12_RESOURCE_DESC::Buffer(numVertices * vertexStride)), m_numVertices(numVertices),
	m_vertexStride(vertexStride), m_vertexBufferView{} {
	CreateVertexBufferView();
}

CEVertexBuffer::CEVertexBuffer(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource, size_t numVertices,
                               size_t vertexStride) : CEBuffer(device, resource), m_numVertices(numVertices),
                                                      m_vertexStride(vertexStride), m_vertexBufferView{} {
	CreateVertexBufferView();
}

CEVertexBuffer::~CEVertexBuffer() {
}

void CEVertexBuffer::CreateVertexBufferView() {
	m_vertexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = static_cast<UINT>(m_numVertices * m_vertexStride);
	m_vertexBufferView.StrideInBytes = static_cast<UINT>(m_vertexStride);
}
