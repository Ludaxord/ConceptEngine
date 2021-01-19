#pragma once
#include <d3d12.h>
#include <DirectXCollision.h>
#include <map>
#include <memory>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	class CEDirectXIndexBuffer;
	class CEDirectXVisitor;
	class CEDirectXCommandList;
	class CEDirectXMaterial;
	class CEDirectXVertexBuffer;
	namespace wrl = Microsoft::WRL;

	class CEDirectXMesh {
	public:
		using BufferMap = std::map<uint32_t, std::shared_ptr<CEDirectXVertexBuffer>>;
		CEDirectXMesh();
		~CEDirectXMesh() = default;

		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);
		D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;

		void SetVertexBuffer(uint32_t slotID, const std::shared_ptr<CEDirectXVertexBuffer>& vertexBuffer);

		/*
		 * Get number if indices are in index buffer
		 * if no index buffer is bound to mesh, function returns 0
		 */
		size_t GetIndexCount() const;

		/*
		 * Get number of vertices in mesh
		 * if mesh does not have vertex buffer, function returns 0
		 */
		size_t GetVertexCount() const;

		void SetMaterial(std::shared_ptr<CEDirectXMaterial> material);
		std::shared_ptr<CEDirectXMaterial> GetMaterial() const;

		/*
		 * Set AABB bounding volume for geometry in mesh
		 */
		void SetAABB(const ::DirectX::BoundingBox& aabb);
		const ::DirectX::BoundingBox& GetAABB() const;

		/*
		 * Draw mesh to CommandList
		 *
		 * @param commandList, command list to draw to.
		 * @param instanceCount, number of instances to draw.
		 * @param startInstance, offset added to instanceID when reading from instance buffers.
		 */
		void Draw(CEDirectXCommandList& commandList, uint32_t instanceCount = 1, uint32_t startInstance = 0);

		/*
		 * Accept visitor
		 */
		void Accept(CEDirectXVisitor& visitor);
	protected:
	private:
		BufferMap m_vertexBuffers;
		std::shared_ptr<CEDirectXIndexBuffer> m_indexBuffer;
		std::shared_ptr<CEDirectXMaterial> m_material;
		D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology;
		::DirectX::BoundingBox m_AABB;
	};
}
