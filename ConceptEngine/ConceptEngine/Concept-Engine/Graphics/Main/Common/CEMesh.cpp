#include "CEMesh.h"

using namespace ConceptEngine::Graphics::Main::Common;

bool CEMesh::Create(const CEMeshData& data) {
	return false;
}

bool CEMesh::BuildAccelerationStructure(ConceptEngine::Graphics::Main::RenderLayer::CECommandList& commandList) {
	return false;
}

CESharedPtr<CEMesh> CEMesh::Make(const CEMeshData& data) {
	return nullptr;
}

void CEMesh::CreateBoundingBox(const CEMeshData& data) {
}
