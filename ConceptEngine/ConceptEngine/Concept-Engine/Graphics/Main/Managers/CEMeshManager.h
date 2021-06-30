#pragma once

#include <string>

#include "../../../Math/CEMath.h"
#include "../../../Core/Containers/CEArray.h"
#include "../../../Utilities/CEHashUtilities.h"
#include "../../../Core/Common/CEManager.h"

namespace ConceptEngine::Graphics::Main {
	struct CEVertex {
		
#if defined(WINDOWS_PLATFORM)
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Tangent;
		DirectX::XMFLOAT2 TexCoord;
#else
		CEVectorFloat3 Position;
		CEVectorFloat3 Normal;
		CEVectorFloat3 Tangent;
		CEVectorFloat2 TexCoord;
#endif
		bool operator==(const CEVertex& o) const {
			return ((Position.x == o.Position.x) && (Position.y == o.Position.y) && (Position.z == o.Position.z)) &&
				((Normal.x == o.Normal.x) && (Normal.y == o.Normal.y) && (Normal.z == o.Normal.z)) &&
				((Tangent.x == o.Tangent.x) && (Tangent.y == o.Tangent.y) && (Tangent.z == o.Tangent.z)) &&
				((TexCoord.x == o.TexCoord.x) && (TexCoord.y == o.TexCoord.y));
		}


	};

	struct CEVertexHasher {
#if defined(WINDOWS_PLATFORM)
		inline size_t operator()(const CEVertex& V) const {
			std::hash<DirectX::XMFLOAT3> Hasher;

			size_t Hash = Hasher(V.Position);
			HashCombine<DirectX::XMFLOAT3>(Hash, V.Normal);
			HashCombine<DirectX::XMFLOAT3>(Hash, V.Tangent);
			HashCombine<DirectX::XMFLOAT2>(Hash, V.TexCoord);

			return Hash;
		}
#else
		inline size_t operator()(const CEVertex& V) const {
			std::hash<CEVectorFloat3> Hasher;

			size_t Hash = Hasher(V.Position);
			HashCombine<CEVectorFloat3>(Hash, V.Normal);
			HashCombine<CEVectorFloat3>(Hash, V.Tangent);
			HashCombine<CEVectorFloat2>(Hash, V.TexCoord);

			return Hash;
		}
#endif
	};

	struct CEMeshData {
		CEArray<CEVertex> Vertices;
		CEArray<uint32> Indices;
	};

	class CEMeshManager : public Core::Common::CEManager {
	public:
		virtual ~CEMeshManager() = default;
		virtual CEMeshData CreateFromFile(const std::string& filename, bool mergeMeshes = true,
		                                  bool leftHanded = true) noexcept = 0;
		virtual CEMeshData CreateCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f) noexcept = 0;
		virtual CEMeshData CreatePlane(uint32 width = 1, uint32 height = 1) noexcept = 0;
		virtual CEMeshData CreateSphere(uint32 subdivisions = 0, float radius = 0.5f) noexcept = 0;
		virtual CEMeshData CreateCone(uint32 sides = 5, float radius = 0.5f, float height = 1.0f) noexcept = 0;
		virtual CEMeshData CreatePyramid() noexcept = 0;
		virtual CEMeshData CreateCylinder(uint32 sides = 5, float radius = 0.5f, float height = 1.0f) noexcept = 0;

		virtual void Subdivide(CEMeshData& data, uint32 subdivisions = 1) noexcept = 0;
		virtual void Optimize(CEMeshData& data, uint32 startVertex = 0) noexcept = 0;
		virtual void CalculateHardNormals(CEMeshData& data) noexcept = 0;
		virtual void CalculateTangent(CEMeshData& data) noexcept = 0;

		bool Create() override {
			return true;
		}

		void Release() {

		};
	};
}
