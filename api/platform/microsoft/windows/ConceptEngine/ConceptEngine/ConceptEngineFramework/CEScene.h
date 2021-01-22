#pragma once
#include <DirectXCollision.h>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <wrl.h>

class aiMaterial;
class aiMesh;
class aiNode;
class aiScene;

namespace Concept::GraphicsEngine::Direct3D {
	class CEDevice;
	class CEMesh;
	class CEMaterial;
	class CEVisitor;
	class CESceneNode;
	class CECommandList;
	namespace wrl = Microsoft::WRL;

	class CEScene {
	public:
		CEScene() = default;
		~CEScene() = default;

		void SetRootNode(std::shared_ptr<CESceneNode> node) {
			m_rootNode = node;
		}

		/**
		 * Get AABB of scene
		 * returns AABB of root node of scene.
		 */
		::DirectX::BoundingBox GetAABB() const;

		/**
		 * Accept visitor.
		 * This will visit scene, then will visit root node of scene.
		 */
		virtual void Accept(CEVisitor& visitor);

	protected:
		friend class CECommandList;

		/**
		 * Load scene from file
		 */
		bool LoadSceneFromFile(CECommandList& commandList, const std::wstring& fileName,
		                       const std::function<bool(float)>& loadingProgress);

		/**
		 * Load scene from string,
		 * Scene can be preloaded into a byte array and scan can be loaded from loaded byte array,
		 * @param scene, byte encoded scene file,
		 * @param format, format of scene file.
		 */
		bool LoadSceneFromString(CECommandList& commandList, const std::string& sceneString,
		                         const std::string& format);

	private:
		void ImportScene(CECommandList& commandList, const aiScene& scene, std::filesystem::path parentPath);
		void ImportMaterial(CECommandList& commandList, const aiMaterial& material,
		                    std::filesystem::path parentPath);
		void ImportMesh(CECommandList& commandList, const aiMesh& mesh);
		std::shared_ptr<CESceneNode> ImportSceneNode(CECommandList& commandList,
		                                                    std::shared_ptr<CESceneNode> parent,
		                                                    const aiNode* aiNode);

		using MaterialMap = std::map<std::string, std::shared_ptr<CEMaterial>>;
		using MaterialList = std::vector<std::shared_ptr<CEMaterial>>;
		using MeshList = std::vector<std::shared_ptr<CEMesh>>;

		MaterialMap m_materialMap;
		MaterialList m_materialList;
		MeshList m_meshList;

		std::shared_ptr<CESceneNode> m_rootNode;
		std::wstring m_sceneFile;
	};
}
