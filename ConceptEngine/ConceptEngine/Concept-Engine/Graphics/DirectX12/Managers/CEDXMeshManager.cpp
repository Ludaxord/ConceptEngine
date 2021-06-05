#include "CEDXMeshManager.h"

using namespace ConceptEngine::Graphics::DirectX12::Managers;

CEDXMeshManager::CEDXMeshManager() = default;

CEDXMeshManager::~CEDXMeshManager() {
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreateFromFile(const std::string& filename, bool mergeMeshes,
                                                                          bool leftHanded) noexcept {
	return {};
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreateCube(float width, float height, float depth) noexcept {
	return {};
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreatePlane(uint32 width, uint32 height) noexcept {
	return {};
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreateSphere(uint32 subdivisions, float radius) noexcept {
	return {};
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::
CreateCone(uint32 sides, float radius, float height) noexcept {
	return {};
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreatePyramid() noexcept {
	return {};
}

ConceptEngine::Graphics::Main::CEMeshData CEDXMeshManager::CreateCylinder(uint32 sides, float radius,
                                                                          float height) noexcept {
	return {};
}

void CEDXMeshManager::Subdivide(Main::CEMeshData& data, uint32 subdivisions) noexcept {
}

void CEDXMeshManager::Optimize(Main::CEMeshData& data, uint32 startVertex) noexcept {
}

void CEDXMeshManager::CalculateHardNormals(Main::CEMeshData& data) noexcept {
}

void CEDXMeshManager::CalculateTangent(Main::CEMeshData& data) noexcept {
}
