#include "CESceneNode.h"
using namespace Concept::GraphicsEngine::Direct3D;

CESceneNode::CESceneNode(const ::DirectX::XMMATRIX& localTransform): m_name("SceneNode"), m_AABB({0, 0, 0}, {0, 0, 0}) {
	m_alignedData = (AlignedData*)_aligned_malloc(sizeof(AlignedData), 16);
	m_alignedData->m_localTransform = localTransform;
	m_alignedData->m_inverseTransform = XMMatrixInverse(nullptr, localTransform);
}

CESceneNode::~CESceneNode() {
	_aligned_free(m_alignedData);
}

const std::string& CESceneNode::GetName() const {
}

void CESceneNode::SetName(const std::string& name) {
}

::DirectX::XMMATRIX CESceneNode::GetLocalTransform() const {
}

void CESceneNode::SetLocalTransform(const ::DirectX::XMMATRIX& localTransform) {
}

::DirectX::XMMATRIX CESceneNode::GetInverseLocalTransform() const {
}

::DirectX::XMMATRIX CESceneNode::GetWorldTransform() const {
}

::DirectX::XMMATRIX CESceneNode::GetInverseWorldTransform() const {
}

void CESceneNode::AddChild(std::shared_ptr<CESceneNode> childNode) {
}

void CESceneNode::RemoveChild(std::shared_ptr<CESceneNode> childNode) {
}

void CESceneNode::SetParent(std::shared_ptr<CESceneNode> parentNode) {
}

size_t CESceneNode::AddMesh(std::shared_ptr<CEMesh> mesh) {
}

void CESceneNode::RemoveMesh(std::shared_ptr<CEMesh> mesh) {
}

std::shared_ptr<CEMesh> CESceneNode::GetMesh(size_t index) {
}

const ::DirectX::BoundingBox& CESceneNode::GetAABB() const {
}

void CESceneNode::Accept(CEVisitor& visitor) {
}

::DirectX::XMMATRIX CESceneNode::GetParentWorldTransform() const {
}
