#include "CEModelLoader.h"

#include <fstream>

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

bool CEModelLoader::LoadM3D(const std::string& fileName, std::vector<Vertex>& vertices, std::vector<USHORT>& indices,
                            std::vector<Subset> subsets, std::vector<Material>& materials) {
	std::ifstream fin(fileName);

	UINT numMaterials = 0;
	UINT numVertices = 0;
	UINT numTriangles = 0;
	UINT numElements = 0;
	UINT numAnimationClips = 0;

	std::string ignore;

	if (fin) {
		fin >> ignore; //file header text
		fin >> ignore >> numMaterials;
		fin >> ignore >> numVertices;
		fin >> ignore >> numTriangles;
		fin >> ignore >> numElements;
		fin >> ignore >> numAnimationClips;

		ReadMaterials(fin, numMaterials, materials);
		ReadSubsetTable(fin, numMaterials, subsets);
		ReadVertices(fin, numVertices, vertices);
		ReadTriangles(fin, numTriangles, indices);

		return true;
	}

	return false;
}

bool CEModelLoader::LoadM3D(const std::string& fileName,
                            std::vector<ModelVertex>& vertices,
                            std::vector<USHORT>& indices,
                            std::vector<Subset>& subsets,
                            std::vector<Material>& materials,
                            CEModelData& modelInfo) {
	std::ifstream fin(fileName);
	UINT numMaterials = 0;
	UINT numVertices = 0;
	UINT numTriangles = 0;
	UINT numElements = 0;
	UINT numAnimationClips = 0;

	std::string ignore;

	if (fin) {
		fin >> ignore; //file header text
		fin >> ignore >> numMaterials;
		fin >> ignore >> numVertices;
		fin >> ignore >> numTriangles;
		fin >> ignore >> numElements;
		fin >> ignore >> numAnimationClips;

		std::vector<DirectX::XMFLOAT4X4> elementOffsets;
		std::vector<int> elementIndexToParentIndex;
		std::unordered_map<std::string, CEAnimationClip> animations;

		ReadMaterials(fin, numMaterials, materials);
		ReadSubsetTable(fin, numMaterials, subsets);
		ReadModelVertices(fin, numVertices, vertices);
		ReadTriangles(fin, numTriangles, indices);
		ReadElementOffsets(fin, numElements, elementOffsets);
		ReadElementHierarchy(fin, numElements, elementIndexToParentIndex);
		ReadAnimationClips(fin, numElements, numAnimationClips, animations);

		modelInfo.Set(elementIndexToParentIndex, elementOffsets, animations);

		return true;
	}


	return false;
}

void CEModelLoader::ReadMaterials(std::ifstream& fin, UINT numMaterials, std::vector<Material>& materials) {
	std::string ignore;
	materials.resize(numMaterials);

	std::string diffuseMapName;
	std::string normalMapName;

	fin >> ignore; //materials header text;
	for (UINT i = 0; i < numMaterials; ++i) {
		fin >> ignore >> materials[i].Name;
		fin >> ignore >> materials[i].DiffuseAlbedo.x >> materials[i].DiffuseAlbedo.y >> materials[i].DiffuseAlbedo.z;
		fin >> ignore >> materials[i].FresnelR0.x >> materials[i].FresnelR0.y >> materials[i].FresnelR0.z;
		fin >> ignore >> materials[i].Roughness;
		fin >> ignore >> materials[i].AlphaClip;
		fin >> ignore >> materials[i].MaterialTypeName;
		fin >> ignore >> materials[i].DiffuseMapName;
		fin >> ignore >> materials[i].NormalMapName;
	}
}

void CEModelLoader::ReadSubsetTable(std::ifstream& fin, UINT numSubsets, std::vector<Subset>& subsets) {
	std::string ignore;
	subsets.resize(numSubsets);

	fin >> ignore; //subset header text;
	for (UINT i = 0; i < numSubsets; ++i) {
		fin >> ignore >> subsets[i].Id;
		fin >> ignore >> subsets[i].VertexStart;
		fin >> ignore >> subsets[i].VertexCount;
		fin >> ignore >> subsets[i].FaceStart;
		fin >> ignore >> subsets[i].FaceCount;
	}
}

void CEModelLoader::ReadVertices(std::ifstream& fin, UINT numVertices, std::vector<Vertex>& vertices) {
	std::string ignore;
	vertices.resize(numVertices);

	fin >> ignore; //vertices header text;
	for (UINT i = 0; i < numVertices; ++i) {
		fin >> ignore >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> ignore >> vertices[i].TangentU.x >> vertices[i].TangentU.y >> vertices[i].TangentU.z >> vertices[i].
			TangentU.w;
		fin >> ignore >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
		fin >> ignore >> vertices[i].TexC.x >> vertices[i].TexC.y;

	}
}

void CEModelLoader::ReadModelVertices(std::ifstream& fin, UINT numVertices, std::vector<ModelVertex>& vertices) {
	std::string ignore;
	vertices.resize(numVertices);

	fin >> ignore; //vertices header text;
	int elementIndices[4];
	float weights[4];
	for (UINT i = 0; i < numVertices; ++i) {
		float skip;
		fin >> ignore >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> ignore >> vertices[i].TangentU.x >> vertices[i].TangentU.y >> vertices[i].TangentU.z >> skip;
		fin >> ignore >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
		fin >> ignore >> vertices[i].TexC.x >> vertices[i].TexC.y;
		fin >> ignore >> weights[0] >> weights[1] >> weights[2] >> weights[3];
		fin >> ignore >> elementIndices[0] >> elementIndices[1] >> elementIndices[2] >> elementIndices[3];

		vertices[i].ElementWeights.x = weights[0];
		vertices[i].ElementWeights.y = weights[1];
		vertices[i].ElementWeights.z = weights[2];

		vertices[i].ElementIndices[0] = (BYTE)elementIndices[0];
		vertices[i].ElementIndices[1] = (BYTE)elementIndices[1];
		vertices[i].ElementIndices[2] = (BYTE)elementIndices[2];
		vertices[i].ElementIndices[3] = (BYTE)elementIndices[3];
	}
}

void CEModelLoader::ReadTriangles(std::ifstream& fin, UINT numTriangles, std::vector<USHORT>& indices) {
	std::string ignore;
	indices.resize(numTriangles * 3);

	fin >> ignore; //triangle header text
	for (UINT i = 0; i < numTriangles; ++i) {
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}
}

void CEModelLoader::ReadElementOffsets(std::ifstream& fin, UINT numElements,
                                       std::vector<DirectX::XMFLOAT4X4>& elementOffsets) {
	std::string ignore;
	elementOffsets.resize(numElements);

	fin >> ignore; //elements header text;
	for (UINT i = 0; i < numElements; ++i) {
		fin >> ignore >>
			elementOffsets[i](0, 0) >> elementOffsets[i](0, 1) >> elementOffsets[i](0, 2) >> elementOffsets[i](0, 3) >>
			elementOffsets[i](1, 0) >> elementOffsets[i](1, 1) >> elementOffsets[i](1, 2) >> elementOffsets[i](1, 3) >>
			elementOffsets[i](2, 0) >> elementOffsets[i](2, 1) >> elementOffsets[i](2, 2) >> elementOffsets[i](2, 3) >>
			elementOffsets[i](3, 0) >> elementOffsets[i](3, 1) >> elementOffsets[i](3, 2) >> elementOffsets[i](3, 3);
	}
}

void CEModelLoader::ReadElementHierarchy(std::ifstream& fin, UINT numElements,
                                         std::vector<int>& elementIndexToParentIndex) {
	std::string ignore;
	elementIndexToParentIndex.resize(numElements);

	fin >> ignore; //ElementHierarchy header text;
	for (UINT i = 0; i < numElements; ++i) {
		fin >> ignore >> elementIndexToParentIndex[i];
	}

}

void CEModelLoader::ReadAnimationClips(std::ifstream& fin, UINT numElements, UINT numAnimationClips,
                                       std::unordered_map<std::string, CEAnimationClip>& animations) {
	std::string ignore;
	fin >> ignore; // AnimationClips header text
	for (UINT clipIndex = 0; clipIndex < numAnimationClips; ++clipIndex) {
		std::string clipName;
		fin >> ignore >> clipName;
		fin >> ignore;


		CEAnimationClip clip;
		clip.Animations.resize(numElements);
		for (UINT elementIndex = 0; elementIndex < numElements; ++elementIndex) {
			ReadElementKeyFrames(fin, numElements, clip.Animations[elementIndex]);
		}
		fin >> ignore;

		animations[clipName] = clip;
	}
}

void CEModelLoader::ReadElementKeyFrames(std::ifstream& fin, UINT numElements, CEAnimation& animation) {
	std::string ignore;
	UINT numKeyFrames = 0;
	fin >> ignore >> ignore >> numKeyFrames;
	fin >> ignore;

	animation.KeyFrames.resize(numKeyFrames);
	for (UINT i = 0; i < numKeyFrames; ++i) {
		float t = 0.0f;

		DirectX::XMFLOAT3 p(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 s(1.0f, 1.0f, 1.0f);
		DirectX::XMFLOAT4 q(0.0f, 0.0f, 0.0f, 1.0f);

		fin >> ignore >> t;
		fin >> ignore >> p.x >> p.y >> p.z;
		fin >> ignore >> s.x >> s.y >> s.z;
		fin >> ignore >> q.x >> q.y >> q.z >> q.w;

		animation.KeyFrames[i].TimePos = t;
		animation.KeyFrames[i].Translation = p;
		animation.KeyFrames[i].Scale = s;
		animation.KeyFrames[i].Rotation = q;
	}

	fin >> ignore;
}
