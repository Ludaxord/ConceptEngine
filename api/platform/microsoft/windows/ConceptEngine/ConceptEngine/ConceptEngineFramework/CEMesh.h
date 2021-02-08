#pragma once
#include <d3d12.h>
#include <DirectXCollision.h>
#include <map>
#include <memory>
#include <wrl.h>

namespace Concept::GraphicsEngine::Direct3D {
	namespace wrl = Microsoft::WRL;

	//TODO: temporary, create separate class for acceleration structure buffers
	struct AccelerationStructureBuffers {
		wrl::ComPtr<ID3D12Resource> pScratch;
		wrl::ComPtr<ID3D12Resource> pResult;
		wrl::ComPtr<ID3D12Resource> pInstanceDesc;
		uint64_t mTlasSize = 0;
	};
	
	class CEIndexBuffer;
	class CEVisitor;
	class CECommandList;
	class CEMaterial;
	class CEVertexBuffer;

	class CEMesh {
	public:
		using BufferMap = std::map<uint32_t, std::shared_ptr<CEVertexBuffer>>;
		CEMesh();
		~CEMesh() = default;

		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);
		D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;

		void SetVertexBuffer(uint32_t slotID, const std::shared_ptr<CEVertexBuffer>& vertexBuffer);
		std::shared_ptr<CEVertexBuffer> GetVertexBuffer(uint32_t slotID) const;

		const BufferMap& GetVertexBuffers() const {
			return m_vertexBuffers;
		}

		void SetIndexBuffer(const std::shared_ptr<CEIndexBuffer>& indexBuffer);
		std::shared_ptr<CEIndexBuffer> GetIndexBuffer();

		void SetBLASBuffer(AccelerationStructureBuffers buffers);
		AccelerationStructureBuffers GetBLASBuffer() const;

		void SetTLASBuffer(AccelerationStructureBuffers buffers);
		AccelerationStructureBuffers GetTLASBuffer() const;
		
		/**
		 * Get number if indices are in index buffer
		 * if no index buffer is bound to mesh, function returns 0
		 */
		size_t GetIndexCount() const;

		/**
		 * Get number of vertices in mesh
		 * if mesh does not have vertex buffer, function returns 0
		 */
		size_t GetVertexCount() const;

		void SetMaterial(std::shared_ptr<CEMaterial> material);
		std::shared_ptr<CEMaterial> GetMaterial() const;

		/**
		 * Set AABB bounding volume for geometry in mesh
		 */
		void SetAABB(const DirectX::BoundingBox& aabb);
		const DirectX::BoundingBox& GetAABB() const;

		/**
		 * Draw mesh to CommandList
		 *
		 * @param commandList, command list to draw to.
		 * @param instanceCount, number of instances to draw.
		 * @param startInstance, offset added to instanceID when reading from instance buffers.
		 */
		void Draw(CECommandList& commandList, uint32_t instanceCount = 1, uint32_t startInstance = 0);

		/**
		 * Accept visitor
		 */
		void Accept(CEVisitor& visitor);
	protected:
	private:
		BufferMap m_vertexBuffers;
		std::shared_ptr<CEIndexBuffer> m_indexBuffer;
		std::shared_ptr<CEMaterial> m_material;
		D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology;
		DirectX::BoundingBox m_AABB;
		AccelerationStructureBuffers m_BLASBuffers;
		AccelerationStructureBuffers m_TLASBuffers;
	};
}
