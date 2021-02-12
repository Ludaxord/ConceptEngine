#include "CED3DSceneVisitor.h"

#include "CECommandList.h"
#include "CEMesh.h"

using namespace ConceptFramework::GraphicsEngine::Direct3D::Visitor;

CED3DSceneVisitor::CED3DSceneVisitor(CECommandList& commandList) : m_commandList(commandList) {
}

void CED3DSceneVisitor::Visit(CEMesh& mesh) {
	mesh.Draw(m_commandList);
}
