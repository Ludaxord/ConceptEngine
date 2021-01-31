#pragma once
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <wrl.h>

namespace Concept::GraphicsEngine::Direct3D {
	class CECommandList;
	class CEVisitor;
	class CEMesh;
	namespace wrl = Microsoft::WRL;

	class CESceneNode : public std::enable_shared_from_this<CESceneNode> {
	public:
		explicit CESceneNode(const ::DirectX::XMMATRIX& localTransform = DirectX::XMMatrixIdentity());
		virtual ~CESceneNode();

		/**
		 * Assign name to scene node so it can be found later.
		 */
		const std::string& GetName() const;
		void SetName(const std::string& name);

		/**
		 * Get scene nodes local (relative to its parent's transform);
		 */
		DirectX::XMMATRIX GetLocalTransform() const;
		void SetLocalTransform(const ::DirectX::XMMATRIX& localTransform);

		/**
		 * Get inverse of local transform.
		 */
		DirectX::XMMATRIX GetInverseLocalTransform() const;

		/**
		 * Get scene node world transform (concatenated with its parents world transform);
		 */
		DirectX::XMMATRIX GetWorldTransform() const;

		/**
		 * Get inverse of world transform (concatenated with its parent world transform)
		 */
		DirectX::XMMATRIX GetInverseWorldTransform() const;

		/**
		 * Add a child node to scene node.
		 * Note: Circular references are not checked.
		 * scene node "own" its children.
		 * If root node is deleted all of children are deleted if nothing else is referencing them
		 */
		void AddChild(std::shared_ptr<CESceneNode> childNode);
		void RemoveChild(std::shared_ptr<CESceneNode> childNode);
		void SetParent(std::shared_ptr<CESceneNode> parentNode);

		/**
		 * Add mesh to scene node.
		 * @return index of mesh in mesh list.
		 */
		size_t AddMesh(std::shared_ptr<CEMesh> mesh);
		void RemoveMesh(std::shared_ptr<CEMesh> mesh);

		/**
		 * Get mesh in list of meshed for called node.
		 */
		std::shared_ptr<CEMesh> GetMesh(size_t index = 0);

		/**
		 * Get AABB for called scene node.
		 * AABB is formed from combination of all mesh AABB's
		 */
		const DirectX::BoundingBox& GetAABB() const;

		/**
		 * Accept a visitor.
		 */
		void Accept(CEVisitor& visitor);

	protected:
		DirectX::XMMATRIX GetParentWorldTransform() const;

	private:
		using NodePointer = std::shared_ptr<CESceneNode>;
		using NodeList = std::vector<NodePointer>;
		using NodeNameMap = std::multimap<std::string, NodePointer>;
		using MeshList = std::vector<std::shared_ptr<CEMesh>>;

		std::string m_name;

		/**
		 * Data must be aligned to 16-byte boundary.
		 * only way to guarantee alignment, is to allocate structure in aligned memory.
		 */
		struct alignas(16) AlignedData {
			DirectX::XMMATRIX m_localTransform;
			DirectX::XMMATRIX m_inverseTransform;
		} * m_alignedData;

		std::weak_ptr<CESceneNode> m_parentNode;
		NodeList m_children;
		NodeNameMap m_childrenByName;
		MeshList m_meshes;

		/**
		 * AABB for this scene node.
		 * Created by merge of AABB of meshes
		 */
		DirectX::BoundingBox m_AABB;
	};
}
