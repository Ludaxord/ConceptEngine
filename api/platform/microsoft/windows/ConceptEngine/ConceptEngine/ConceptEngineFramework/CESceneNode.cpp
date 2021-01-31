#include "CESceneNode.h"


#include "CEMesh.h"
#include "CEVisitor.h"
using namespace Concept::GraphicsEngine::Direct3D;

CESceneNode::CESceneNode(const DirectX::XMMATRIX& localTransform)
	: m_name("SceneNode")
	  , m_AABB({0, 0, 0}, {0, 0, 0}) {
	m_alignedData = (AlignedData*)_aligned_malloc(sizeof(AlignedData), 16);
	m_alignedData->m_localTransform = localTransform;
	m_alignedData->m_inverseTransform = XMMatrixInverse(nullptr, localTransform);
}

CESceneNode::~CESceneNode() {
	_aligned_free(m_alignedData);
}

const std::string& CESceneNode::GetName() const {
	return m_name;
}

void CESceneNode::SetName(const std::string& name) {
	m_name = name;
}

DirectX::XMMATRIX CESceneNode::GetLocalTransform() const {
	return m_alignedData->m_localTransform;
}

void CESceneNode::SetLocalTransform(const DirectX::XMMATRIX& localTransform) {
	m_alignedData->m_localTransform = localTransform;
	m_alignedData->m_inverseTransform = XMMatrixInverse(nullptr, localTransform);
}

DirectX::XMMATRIX CESceneNode::GetInverseLocalTransform() const {
	return m_alignedData->m_inverseTransform;
}

DirectX::XMMATRIX CESceneNode::GetWorldTransform() const {
	return m_alignedData->m_localTransform * GetParentWorldTransform();
}

DirectX::XMMATRIX CESceneNode::GetInverseWorldTransform() const {
	return XMMatrixInverse(nullptr, GetWorldTransform());
}

DirectX::XMMATRIX CESceneNode::GetParentWorldTransform() const {
	DirectX::XMMATRIX parentTransform = DirectX::XMMatrixIdentity();
	if (auto parentNode = m_parentNode.lock()) {
		parentTransform = parentNode->GetWorldTransform();
	}

	return parentTransform;
}

void CESceneNode::AddChild(std::shared_ptr<CESceneNode> childNode) {
	if (childNode) {
		NodeList::iterator iter = std::find(m_children.begin(), m_children.end(), childNode);
		if (iter == m_children.end()) {
			DirectX::XMMATRIX worldTransform = childNode->GetWorldTransform();
			childNode->m_parentNode = shared_from_this();
			DirectX::XMMATRIX localTransform = worldTransform * GetInverseWorldTransform();
			childNode->SetLocalTransform(localTransform);
			m_children.push_back(childNode);
			if (!childNode->GetName().empty()) {
				m_childrenByName.emplace(childNode->GetName(), childNode);
			}
		}
	}
}

void CESceneNode::RemoveChild(std::shared_ptr<CESceneNode> childNode) {
	if (childNode) {
		NodeList::const_iterator iter = std::find(m_children.begin(), m_children.end(), childNode);
		if (iter != m_children.cend()) {
			childNode->SetParent(nullptr);
			m_children.erase(iter);

			// Also remove it from the name map.
			NodeNameMap::iterator iter2 = m_childrenByName.find(childNode->GetName());
			if (iter2 != m_childrenByName.end()) {
				m_childrenByName.erase(iter2);
			}
		}
		else {
			// Maybe the child appears deeper in the scene graph.
			for (auto child : m_children) {
				child->RemoveChild(childNode);
			}
		}
	}
}

void CESceneNode::SetParent(std::shared_ptr<CESceneNode> parentNode) {
	// Parents own their children.. If this node is not owned
	// by anyone else, it will cease to exist if we remove it from it's parent.
	// As a precaution, store myself as a shared pointer so I don't get deleted
	// half-way through this function!
	// Technically self deletion shouldn't occur because the thing invoking this function
	// should have a shared_ptr to it.
	std::shared_ptr<CESceneNode> me = shared_from_this();
	if (parentNode) {
		parentNode->AddChild(me);
	}
	else if (auto parent = m_parentNode.lock()) {
		// Setting parent to NULL.. remove from current parent and reset parent node.
		auto worldTransform = GetWorldTransform();
		parent->RemoveChild(me);
		m_parentNode.reset();
		SetLocalTransform(worldTransform);
	}
}

size_t CESceneNode::AddMesh(std::shared_ptr<CEMesh> mesh) {
	size_t index = (size_t)-1;
	if (mesh) {
		MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
		if (iter == m_meshes.cend()) {
			index = m_meshes.size();
			m_meshes.push_back(mesh);

			// Merge the mesh's AABB with AABB of the scene node.
			DirectX::BoundingBox::CreateMerged(m_AABB, m_AABB, mesh->GetAABB());
		}
		else {
			index = iter - m_meshes.begin();
		}
	}

	return index;
}

void CESceneNode::RemoveMesh(std::shared_ptr<CEMesh> mesh) {
	if (mesh) {
		MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
		if (iter != m_meshes.end()) {
			m_meshes.erase(iter);
		}
	}
}

std::shared_ptr<CEMesh> CESceneNode::GetMesh(size_t pos) {
	std::shared_ptr<CEMesh> mesh = nullptr;

	if (pos < m_meshes.size()) {
		mesh = m_meshes[pos];
	}

	return mesh;
}

const DirectX::BoundingBox& CESceneNode::GetAABB() const {
	return m_AABB;
}

void CESceneNode::Accept(CEVisitor& visitor) {
	visitor.Visit(*this);

	// Visit meshes
	for (auto& mesh : m_meshes) {
		mesh->Accept(visitor);
	}

	// Visit children
	for (auto& child : m_children) {
		child->Accept(visitor);
	}
}
