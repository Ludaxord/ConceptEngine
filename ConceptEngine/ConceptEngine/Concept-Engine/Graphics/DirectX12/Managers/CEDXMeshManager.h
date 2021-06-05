#pragma once

#include "../../Main/Managers/CEMeshManager.h"

namespace ConceptEngine::Graphics::DirectX12::Managers {
	class CEDXMeshManager final : public Main::CEMeshManager {
	public:
		CEDXMeshManager();
		~CEDXMeshManager() override;
		Main::CEMeshData
		CreateFromFile(const std::string& filename, bool mergeMeshes, bool leftHanded) noexcept override;
		Main::CEMeshData CreateCube(float width, float height, float depth) noexcept override;
		Main::CEMeshData CreatePlane(uint32 width, uint32 height) noexcept override;
		Main::CEMeshData CreateSphere(uint32 subdivisions, float radius) noexcept override;
		Main::CEMeshData CreateCone(uint32 sides, float radius, float height) noexcept override;
		Main::CEMeshData CreatePyramid() noexcept override;
		Main::CEMeshData CreateCylinder(uint32 sides, float radius, float height) noexcept override;
		void Subdivide(Main::CEMeshData& data, uint32 subdivisions) noexcept override;
		void Optimize(Main::CEMeshData& data, uint32 startVertex) noexcept override;
		void CalculateHardNormals(Main::CEMeshData& data) noexcept override;
		void CalculateTangent(Main::CEMeshData& data) noexcept override;
	};
}
