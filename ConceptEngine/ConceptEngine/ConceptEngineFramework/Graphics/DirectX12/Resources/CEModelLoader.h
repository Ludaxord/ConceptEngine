#pragma once
#include <DirectXMath.h>
#include <intsafe.h>
#include <string>
#include <vector>

#include "CEModel.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	enum class ModelExtension {
		M3D = 0,
		OBJ,
		STL,
		FBX,
		DAE,
		MAX3DS,
		BLENDER,
		MAYA
	};

	class CEModelLoader {
	public:
		struct Vertex {
			DirectX::XMFLOAT3 Pos;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TexC;
			DirectX::XMFLOAT4 TangentU;
		};

		struct ModelVertex {
			DirectX::XMFLOAT3 Pos;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TexC;
			DirectX::XMFLOAT4 TangentU;
			DirectX::XMFLOAT3 ElementWeights;
			BYTE ElementIndices[4];
		};

		struct Subset {
			UINT Id = -1;
			UINT VertexStart = 0;
			UINT VertexCount = 0;
			UINT FaceStart = 0;
			UINT FaceCount = 0;
		};

		struct Material {
			std::string Name;

			DirectX::XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
			DirectX::XMFLOAT3 FresnelR0 = {0.01f, 0.01f, 0.01f};
			float Roughness = 0.8f;
			bool AlphaClip = false;

			std::string MaterialTypeName;
			std::string DiffuseMapName;
			std::string NormalMapName;
		};

		bool LoadM3D(const std::string& fileName,
		             std::vector<Vertex>& vertices,
		             std::vector<USHORT>& indices,
		             std::vector<Subset> subsets,
		             std::vector<Material>& materials);
		bool LoadM3D(const std::string& fileName,
		             std::vector<ModelVertex>& vertices,
		             std::vector<USHORT>& indices,
		             std::vector<Subset>& subsets,
		             std::vector<Material>& materials,
		             CEModelData& modelInfo);
	protected:
	private:
		void ReadMaterials(std::ifstream& fin, UINT numMaterials, std::vector<Material>& materials);
		void ReadSubsetTable(std::ifstream& fin, UINT numSubsets, std::vector<Subset>& subsets);
		void ReadVertices(std::ifstream& fin, UINT numVertices, std::vector<Vertex>& vertices);
		void ReadModelVertices(std::ifstream& fin, UINT numVertices, std::vector<ModelVertex>& vertices);
		void ReadTriangles(std::ifstream& fin, UINT numTriangles, std::vector<USHORT>& indices);
		void ReadElementOffsets(std::ifstream& fin, UINT numElements, std::vector<DirectX::XMFLOAT4X4>& elementOffsets);
		void ReadElementHierarchy(std::ifstream& fin, UINT numElements, std::vector<int>& elementIndexToParentIndex);
		void ReadAnimationClips(std::ifstream& fin,
		                        UINT numElements,
		                        UINT numAnimationClips,
		                        std::unordered_map<std::string, CEAnimationClip>& animations);
		void ReadElementKeyFrames(std::ifstream& fin, UINT numElements, CEAnimation& animation);
	};
}
