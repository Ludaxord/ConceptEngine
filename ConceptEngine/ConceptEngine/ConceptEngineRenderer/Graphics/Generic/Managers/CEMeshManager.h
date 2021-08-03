#pragma once
#include "../Managers/CEManager.h"

struct MeshData;

class CEMeshManager : public CEManager {
public:
	virtual MeshData CreateFromFile(const std::string& filename, bool mergeMeshes = true,
	                                  bool leftHanded = true) noexcept = 0;
	virtual MeshData CreateCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f) noexcept = 0;
	virtual MeshData CreatePlane(uint32 width = 1, uint32 height = 1) noexcept = 0;
	virtual MeshData CreateSphere(uint32 subdivisions = 0, float radius = 0.5f) noexcept = 0;
	virtual MeshData CreateCone(uint32 sides = 5, float radius = 0.5f, float height = 1.0f) noexcept = 0;
	virtual MeshData CreatePyramid() noexcept = 0;
	virtual MeshData CreateCylinder(uint32 sides = 5, float radius = 0.5f, float height = 1.0f) noexcept = 0;

	virtual void Subdivide(MeshData& data, uint32 subdivisions = 1) noexcept = 0;
	virtual void Optimize(MeshData& data, uint32 startVertex = 0) noexcept = 0;
	virtual void CalculateHardNormals(MeshData& data) noexcept = 0;
	virtual void CalculateTangents(MeshData& data) noexcept = 0;

	bool Create() override {
		return true;
	}

	void Release() {

	};
};
