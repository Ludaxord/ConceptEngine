#pragma once
#include "../../../Graphics/Generic/Managers/CEMeshManager.h"
#include "Utilities/MatrixUtilities.h"

struct Vertex {
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	XMFLOAT2 TexCoord;

	FORCEINLINE bool operator==(const Vertex& Other) const {
		return
			((Position.x == Other.Position.x) && (Position.y == Other.Position.y) && (Position.z == Other.Position.z))
			&&
			((Normal.x == Other.Normal.x) && (Normal.y == Other.Normal.y) && (Normal.z == Other.Normal.z)) &&
			((Tangent.x == Other.Tangent.x) && (Tangent.y == Other.Tangent.y) && (Tangent.z == Other.Tangent.z)) &&
			((TexCoord.x == Other.TexCoord.x) && (TexCoord.y == Other.TexCoord.y));
	}
};

struct VertexHasher {
	inline size_t operator()(const Vertex& V) const {
		std::hash<XMFLOAT3> Hasher;

		size_t Hash = Hasher(V.Position);
		HashCombine<XMFLOAT3>(Hash, V.Normal);
		HashCombine<XMFLOAT3>(Hash, V.Tangent);
		HashCombine<XMFLOAT2>(Hash, V.TexCoord);

		return Hash;
	}
};

struct MeshData {
	CEArray<Vertex> Vertices;
	CEArray<uint32> Indices;
	XMFLOAT4X4 Transform = SetFloat4X4(1.0f);
};

class CEDX12MeshManager : public CEMeshManager {
public:
	CEDX12MeshManager();
	~CEDX12MeshManager();
	MeshData CreateFromFile(const std::string& Filename, bool MergeMeshes = true,
	                        bool LeftHanded = true) noexcept override;
	MeshData CreateCube(float Width = 1.0f, float Height = 1.0f, float Depth = 1.0f) noexcept override;
	MeshData CreatePlane(uint32 Width = 1, uint32 Height = 1) noexcept override;
	MeshData CreateSphere(uint32 Subdivisions = 0, float Radius = 0.5f) noexcept override;
	MeshData CreateCone(uint32 Sides = 5, float Radius = 0.5f, float Height = 1.0f) noexcept override;
	//static MeshData createTorus() noexcept;
	//static MeshData createTeapot() noexcept;
	MeshData CreatePyramid() noexcept override;
	MeshData CreateCylinder(uint32 Sides = 5, float Radius = 0.5f, float Height = 1.0f) noexcept override;

	void Subdivide(MeshData& OutData, uint32 Subdivisions = 1) noexcept override;
	void Optimize(MeshData& OutData, uint32 StartVertex = 0) noexcept override;
	void CalculateHardNormals(MeshData& OutData) noexcept override;
	void CalculateTangents(MeshData& OutData) noexcept override;
};
