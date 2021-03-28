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
}

bool CEModelLoader::LoadM3D(const std::string& fileName, std::vector<ModelVertex>& vertices,
                            std::vector<USHORT>& indices, std::vector<Subset>& subsets,
                            std::vector<Material>& materials,
                            CEModelData& modelInfo) {
}

void CEModelLoader::ReadMaterials(std::ifstream& fin, UINT numMaterials, std::vector<Material>& materials) {
}

void CEModelLoader::ReadSubsetTable(std::ifstream& fin, UINT numSubsets, std::vector<Subset>& subsets) {
}

void CEModelLoader::ReadVertices(std::ifstream& fin, UINT numVertices, std::vector<Vertex>& vertices) {
}

void CEModelLoader::ReadModelVertices(std::ifstream& fin, UINT numVertices, std::vector<ModelVertex>& vertices) {
}

void CEModelLoader::ReadTriangles(std::ifstream& fin, UINT numTriangles, std::vector<USHORT>& indices) {
}

void CEModelLoader::ReadElementOffsets(std::ifstream& fin, UINT numElements,
                                       std::vector<DirectX::XMFLOAT4X4>& elementOffsets) {
}

void CEModelLoader::ReadElementHierarchy(std::ifstream& fin, UINT numElements,
                                         std::vector<int>& elementIndexToParentIndex) {
}

void CEModelLoader::ReadAnimationClips(std::ifstream& fin, UINT numElements, UINT numAnimationClips,
                                       std::unordered_map<std::string, CEAnimationClip>& animations) {
}

void CEModelLoader::ReadElementKeyFrames(std::ifstream& fin, UINT numElements, CEAnimation& animation) {
}
