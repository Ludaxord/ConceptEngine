#include "CEScene.h"


#include "CECommandList.h"
#include "CEDX12Libs.h"
#include "CEMaterial.h"
#include "CEMesh.h"
#include "CESceneNode.h"
#include "CETexture.h"
#include "CEVertexTypes.h"
#include "CEVisitor.h"

using namespace Concept::GraphicsEngine::Direct3D;

/*
 * Progress handler for assimp library
 */
class ProgressHandler : public Assimp::ProgressHandler {
public:
	ProgressHandler(const CEScene& scene, const std::function<bool(float)> progressCallback): m_scene(scene),
		m_progressCallback(progressCallback) {

	}

	virtual bool Update(float percentage) override {
		/*
		 * Invoke progress callback
		 */
		if (m_progressCallback) {
			return m_progressCallback(percentage);
		}

		return true;
	}

protected:
private:
	const CEScene& m_scene;
	std::function<bool(float)> m_progressCallback;
};

/*
 * Helper function to create DirectX::BoundingBox from an aiAABB
 */
inline DirectX::BoundingBox CreateBoundingBox(const aiAABB& aabb) {
	XMVECTOR min = XMVectorSet(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z, 1.0f);
	XMVECTOR max = XMVectorSet(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z, 1.0f);

	DirectX::BoundingBox bb;
	BoundingBox::CreateFromPoints(bb, min, max);

	return bb;
}


::DirectX::BoundingBox CEScene::GetAABB() const {
	DirectX::BoundingBox aabb{{0, 0, 0}, {0, 0, 0}};
	if (m_rootNode) {
		aabb = m_rootNode->GetAABB();
	}

	return aabb;
}

void CEScene::Accept(CEVisitor& visitor) {
	visitor.Visit(*this);
	if (m_rootNode) {
		m_rootNode->Accept(visitor);
	}
}

bool CEScene::LoadSceneFromFile(CECommandList& commandList, const std::wstring& fileName,
                                const std::function<bool(float)>& loadingProgress) {
	fs::path filePath = fileName;
	fs::path exportPath = fs::path(filePath).replace_extension("assbin");

	fs::path parentPath;
	if (filePath.has_parent_path()) {
		parentPath = filePath.parent_path();
	}
	else {
		parentPath = fs::current_path();
	}

	Assimp::Importer importer;
	const aiScene* scene;

	importer.SetProgressHandler(new ProgressHandler(*this, loadingProgress));

	/*
	 * Check if preprocessed file exists.
	 */
	if (fs::exists(exportPath) && fs::is_regular_file(exportPath)) {
		scene = importer.ReadFile(exportPath.string(), aiProcess_GenBoundingBoxes);
	}
	else {
		/*
		 * File has not been preprocessed yet. Import and processes file
		 */
		importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

		unsigned int preprocessFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_OptimizeGraph |
			aiProcess_ConvertToLeftHanded | aiProcess_GenBoundingBoxes;
		scene = importer.ReadFile(filePath.string(), preprocessFlags);

		if (scene) {
			/*
			 * Export preprocessed scene file for faster loading next time.
			 */
			Assimp::Exporter exporter;
			exporter.Export(scene, "assbin", exportPath.string(), 0);
		}
	}

	if (!scene) {
		return false;
	}

	ImportScene(commandList, *scene, parentPath);

	return true;
}

bool CEScene::LoadSceneFromString(CECommandList& commandList, const std::string& sceneString,
                                  const std::string& format) {
	Assimp::Importer importer;
	const aiScene* scene = nullptr;

	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

	unsigned int preprocessFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded |
		aiProcess_GenBoundingBoxes;

	scene = importer.ReadFileFromMemory(sceneString.data(), sceneString.size(), preprocessFlags, format.c_str());

	if (!scene) {
		return false;
	}

	ImportScene(commandList, *scene, fs::current_path());

	return true;
}

void CEScene::ImportScene(CECommandList& commandList, const aiScene& scene, std::filesystem::path parentPath) {
	if (m_rootNode) {
		m_rootNode.reset();
	}
	m_materialMap.clear();
	m_materialList.clear();
	m_meshList.clear();

	/*
	 * Import scene materials
	 */
	for (unsigned int i = 0; i < scene.mNumMaterials; ++i) {
		ImportMaterial(commandList, *(scene.mMaterials[i]), parentPath);
	}

	/*
	 * Import meshes
	 */
	for (unsigned int i = 0; i < scene.mNumMeshes; ++i) {
		ImportMesh(commandList, *(scene.mMeshes[i]));
	}

	/*
	 * Import root node
	 */
	m_rootNode = ImportSceneNode(commandList, nullptr, scene.mRootNode);
}

void CEScene::ImportMaterial(CECommandList& commandList, const aiMaterial& material, std::filesystem::path parentPath) {
	aiString materialName;
	aiString aiTexturePath;
	aiTextureOp aiBlendOperation;
	float blendFactor;
	aiColor4D diffuseColor;
	aiColor4D specularColor;
	aiColor4D ambientColor;
	aiColor4D emissiveColor;
	float opacity;
	float indexOfRefraction;
	float reflectivity;
	float shininess;
	float bumpIntensity;

	std::shared_ptr<CEMaterial> pMaterial = std::make_shared<CEMaterial>();

	if (material.Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS) {
		pMaterial->SetAmbientColor(XMFLOAT4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a));
	}

	if (material.Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == aiReturn_SUCCESS) {
		pMaterial->SetEmissiveColor(XMFLOAT4(emissiveColor.r, emissiveColor.g, emissiveColor.b, emissiveColor.a));
	}

	if (material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == aiReturn_SUCCESS) {
		pMaterial->SetDiffuseColor(XMFLOAT4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a));
	}

	if (material.Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == aiReturn_SUCCESS) {
		pMaterial->SetSpecularColor(XMFLOAT4(specularColor.r, specularColor.g, specularColor.b, specularColor.a));
	}

	if (material.Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS) {
		pMaterial->SetAmbientColor(XMFLOAT4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a));
	}

	if (material.Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
		pMaterial->SetSpecularPower(shininess);
	}

	if (material.Get(AI_MATKEY_OPACITY, opacity) == aiReturn_SUCCESS) {
		pMaterial->SetOpacity(opacity);
	}

	if (material.Get(AI_MATKEY_REFRACTI, indexOfRefraction) == aiReturn_SUCCESS) {
		pMaterial->SetIndexOfRefraction(indexOfRefraction);
	}

	if (material.Get(AI_MATKEY_REFLECTIVITY, reflectivity) == aiReturn_SUCCESS) {
		pMaterial->SetReflectance(XMFLOAT4(reflectivity, reflectivity, reflectivity, reflectivity));
	}

	if (material.Get(AI_MATKEY_BUMPSCALING, bumpIntensity) == aiReturn_SUCCESS) {
		pMaterial->SetBumpIntensity(bumpIntensity);
	}

	/*
	 * Load ambient textures
	 */
	if (material.GetTextureCount(aiTextureType_AMBIENT) > 0 && material.GetTexture(
			aiTextureType_AMBIENT, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) ==
		aiReturn_SUCCESS
	) {
		fs::path texturePath(aiTexturePath.C_Str());
		auto texture = commandList.LoadTextureFromFile(parentPath / texturePath, true);
		pMaterial->SetTexture(CEMaterial::TextureType::Ambient, texture);
	}

	/*
	 * Load emissive textures
	 */
	if (material.GetTextureCount(aiTextureType_EMISSIVE) > 0 && material.GetTexture(
			aiTextureType_EMISSIVE, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) ==
		aiReturn_SUCCESS
	) {
		fs::path texturePath(aiTexturePath.C_Str());
		auto texture = commandList.LoadTextureFromFile(parentPath / texturePath, true);
		pMaterial->SetTexture(CEMaterial::TextureType::Emissive, texture);
	}

	/*
	 * Load diffuse textures
	 */
	if (material.GetTextureCount(aiTextureType_DIFFUSE) > 0 && material.GetTexture(
			aiTextureType_DIFFUSE, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) ==
		aiReturn_SUCCESS
	) {
		fs::path texturePath(aiTexturePath.C_Str());
		auto texture = commandList.LoadTextureFromFile(parentPath / texturePath, true);
		pMaterial->SetTexture(CEMaterial::TextureType::Diffuse, texture);
	}

	/*
	 * Load specular textures
	 */
	if (material.GetTextureCount(aiTextureType_SPECULAR) > 0 && material.GetTexture(
			aiTextureType_SPECULAR, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) ==
		aiReturn_SUCCESS
	) {
		fs::path texturePath(aiTexturePath.C_Str());
		auto texture = commandList.LoadTextureFromFile(parentPath / texturePath, true);
		pMaterial->SetTexture(CEMaterial::TextureType::Specular, texture);
	}

	/*
	 * Load specular power textures
	 */
	if (material.GetTextureCount(aiTextureType_SHININESS) > 0 && material.GetTexture(
			aiTextureType_SHININESS, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) ==
		aiReturn_SUCCESS
	) {
		fs::path texturePath(aiTexturePath.C_Str());
		auto texture = commandList.LoadTextureFromFile(parentPath / texturePath, true);
		pMaterial->SetTexture(CEMaterial::TextureType::SpecularPower, texture);
	}

	if (material.GetTextureCount(aiTextureType_OPACITY) > 0 && material.GetTexture(
			aiTextureType_OPACITY, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) ==
		aiReturn_SUCCESS
	) {
		fs::path texturePath(aiTexturePath.C_Str());
		auto texture = commandList.LoadTextureFromFile(parentPath / texturePath, false);
		pMaterial->SetTexture(CEMaterial::TextureType::Opacity, texture);
	}

	/*
	 * Load ambient textures
	 */
	if (material.GetTextureCount(aiTextureType_NORMALS) > 0 && material.GetTexture(
		aiTextureType_NORMALS, 0, &aiTexturePath) == aiReturn_SUCCESS) {
		fs::path texturePath(aiTexturePath.C_Str());
		auto texture = commandList.LoadTextureFromFile(parentPath / texturePath, false);
		pMaterial->SetTexture(CEMaterial::TextureType::Normal, texture);
	}
		/*
		 * Load bump map (only if there is no normal map). 
		 */
	else if (material.GetTextureCount(aiTextureType_HEIGHT) > 0 && material.GetTexture(
		aiTextureType_HEIGHT, 0, &aiTexturePath, nullptr, nullptr, &blendFactor) == aiReturn_SUCCESS) {

		fs::path texturePath(aiTexturePath.C_Str());
		auto texture = commandList.LoadTextureFromFile(parentPath / texturePath, false);
		/*
		 * Some materals actually store normal maps in bump map slot.
		 * Assimp can not tell difference between there two types of textures,
		 * so there is need to make an assumption about whether texture is normal map or a map based on its pixel depth.
		 * bump maps are usually 8BPP (grayscale) and normal maps are usually 24 BPP or higher
		 */
		CEMaterial::TextureType textureType = (texture->BitsPerPixel() >= 24)
			                                      ? CEMaterial::TextureType::Normal
			                                      : CEMaterial::TextureType::Bump;
		pMaterial->SetTexture(textureType, texture);
	}

	m_materialList.push_back(pMaterial);
}

void CEScene::ImportMesh(CECommandList& commandList, const aiMesh& aiMesh) {
	auto mesh = std::make_shared<CEMesh>();
	std::vector<CEVertexPositionNormalTangentBitangentTexture> vertexData(aiMesh.mNumVertices);

	assert(aiMesh.mMaterialIndex < m_materialList.size());
	mesh->SetMaterial(m_materialList[aiMesh.mMaterialIndex]);

	unsigned int i;
	if (aiMesh.HasPositions()) {
		for (i = 0; i < aiMesh.mNumVertices; ++i) {
			vertexData[i].Position = {aiMesh.mVertices[i].x, aiMesh.mVertices[i].y, aiMesh.mVertices[i].z};
		}
	}

	if (aiMesh.HasNormals()) {
		for (i = 0; i < aiMesh.mNumVertices; ++i) {
			vertexData[i].Normal = {aiMesh.mNormals[i].x, aiMesh.mNormals[i].y, aiMesh.mNormals[i].z};
		}
	}

	if (aiMesh.HasTangentsAndBitangents()) {
		for (i = 0; i < aiMesh.mNumVertices; ++i) {
			vertexData[i].Tangent = {aiMesh.mTangents[i].x, aiMesh.mTangents[i].y, aiMesh.mTangents[i].z};
			vertexData[i].Bitangent = {
				aiMesh.mBitangents[i].x, aiMesh.mBitangents[i].y, aiMesh.mBitangents[i].z
			};
		}
	}

	if (aiMesh.HasTextureCoords(0)) {
		for (i = 0; i < aiMesh.mNumVertices; ++i) {
			vertexData[i].TexCoord = {
				aiMesh.mTextureCoords[0][i].x, aiMesh.mTextureCoords[0][i].y, aiMesh.mTextureCoords[0][i].z
			};
		}
	}

	auto vertexBuffer = commandList.CopyVertexBuffer(vertexData);
	mesh->SetVertexBuffer(0, vertexBuffer);

	/*
	 * Extract index buffer
	 */
	if (aiMesh.HasFaces()) {
		std::vector<unsigned int> indices;
		for (i = 0; i < aiMesh.mNumFaces; ++i) {
			const aiFace& face = aiMesh.mFaces[i];
			/*
			 * Only extract triangular faces
			 */
			if (face.mNumIndices == 3) {
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
		}

		if (indices.size() > 0) {
			auto indexBuffer = commandList.CopyIndexBuffer(indices);
			mesh->SetIndexBuffer(indexBuffer);
		}
	}

	/*
	 * Set AABB from AI Mesh's AABB.
	 */
	mesh->SetAABB(CreateBoundingBox(aiMesh.mAABB));
	m_meshList.push_back(mesh);
}

std::shared_ptr<CESceneNode> CEScene::ImportSceneNode(CECommandList& commandList,
                                                      std::shared_ptr<CESceneNode> parent,
                                                      const aiNode* aiNode) {
	if (!aiNode) {
		return nullptr;
	}

	auto node = std::make_shared<CESceneNode>(XMMATRIX(&(aiNode->mTransformation.a1)));
	node->SetParent(parent);

	if (aiNode->mName.length > 0) {
		node->SetName(aiNode->mName.C_Str());
	}

	/*
	 * Add meshes to scene node
	 */
	for (unsigned int i = 0; i < aiNode->mNumMeshes; ++i) {
		assert(aiNode->mMeshes[i] < m_meshList.size());
		std::shared_ptr<CEMesh> pMesh = m_meshList[aiNode->mMeshes[i]];
		node->AddMesh(pMesh);
	}

	/*
	 * Recursively Import children
	 */
	for (unsigned int i = 0; i < aiNode->mNumChildren; ++i) {
		auto child = ImportSceneNode(commandList, node, aiNode->mChildren[i]);
		node->AddChild(child);
	}

	return node;
}
