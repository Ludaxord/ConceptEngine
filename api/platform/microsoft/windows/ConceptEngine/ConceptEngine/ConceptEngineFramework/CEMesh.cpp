#include "CEMesh.h"


#include "CECommandList.h"
#include "CEIndexBuffer.h"
#include "CEVertexBuffer.h"
#include "CEVisitor.h"
using namespace Concept::GraphicsEngine::Direct3D;

CEMesh::CEMesh(): m_primitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
}

void CEMesh::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) {
	m_primitiveTopology = primitiveTopology;
}

D3D12_PRIMITIVE_TOPOLOGY CEMesh::GetPrimitiveTopology() const {
	return m_primitiveTopology;
}

void CEMesh::SetVertexBuffer(uint32_t slotID, const std::shared_ptr<CEVertexBuffer>& vertexBuffer) {
	m_vertexBuffers[slotID] = vertexBuffer;
}

std::shared_ptr<CEVertexBuffer> CEMesh::GetVertexBuffer(uint32_t slotID) const {
	auto iter = m_vertexBuffers.find(slotID);
	auto vertexBuffer = iter != m_vertexBuffers.end() ? iter->second : nullptr;

	return vertexBuffer;
}

void CEMesh::SetIndexBuffer(const std::shared_ptr<CEIndexBuffer>& indexBuffer) {
	m_indexBuffer = indexBuffer;
}

std::shared_ptr<CEIndexBuffer> CEMesh::GetIndexBuffer() {
	return m_indexBuffer;
}

size_t CEMesh::GetIndexCount() const {
	size_t indexCount = 0;
	if (m_indexBuffer) {
		indexCount = m_indexBuffer->GetNumIndices();
	}
	return indexCount;
}

size_t CEMesh::GetVertexCount() const {
	size_t vertexCount = 0;
	/*
	 * Count number of vertices in mesh, just take number of vertices in first vertex buffer;
	 */
	BufferMap::const_iterator iter = m_vertexBuffers.cbegin();
	if (iter != m_vertexBuffers.cend()) {
		vertexCount = iter->second->GetNumVertices();
	}
	return vertexCount;
}

void CEMesh::SetMaterial(std::shared_ptr<CEMaterial> material) {
	m_material = material;
}

std::shared_ptr<CEMaterial> CEMesh::GetMaterial() const {
	return m_material;
}

void CEMesh::SetAABB(const ::DirectX::BoundingBox& aabb) {
	m_AABB = aabb;
}

const ::DirectX::BoundingBox& CEMesh::GetAABB() const {
	return m_AABB;
}

void CEMesh::Draw(CECommandList& commandList, uint32_t instanceCount, uint32_t startInstance) {
	commandList.SetPrimitiveTopology(GetPrimitiveTopology());
	for (auto vertexBuffer : m_vertexBuffers) {
		commandList.SetVertexBuffer(vertexBuffer.first, vertexBuffer.second);
	}

	auto indexCount = GetIndexCount();
	auto vertexCount = GetVertexCount();

	if (indexCount > 0) {
		commandList.SetIndexBuffer(m_indexBuffer);
		commandList.DrawIndexed(indexCount, instanceCount, 0u, 0u, startInstance);
	}
	else if (vertexCount > 0) {
		commandList.Draw(vertexCount, instanceCount, 0u, startInstance);
	}
}

void CEMesh::Accept(CEVisitor& visitor) {
	visitor.Visit(*this);
}
