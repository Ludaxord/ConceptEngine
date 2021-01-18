#pragma once
#include <DirectXCollision.h>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <wrl.h>

class aiMaterial;
class aiMesh;
class aiNode;
class aiScene;

namespace ConceptEngine::GraphicsEngine::DirectX {
	class CEDirectXVisitor;
	class CEDirectXSceneNode;
	class CEDirectXCommandList;
	namespace wrl = Microsoft::WRL;

	class CEDirectXScene {
	public:
		CEDirectXScene() = default;
		~CEDirectXScene() = default;

		void SetRootNode(std::shared_ptr<CEDirectXSceneNode> node) {
			m_rootNode = node;
		}

		/*
		 * Get AABB of scene
		 * returns AABB of root node of scene.
		 */
		::DirectX::BoundingBox GetAABB() const;

		/*
		 * Accept visitor.
		 * This will visit scene, then will visit root node of scene.
		 */
		virtual void Accept(CEDirectXVisitor& visitor);

	protected:
		friend class CEDirectXCommandList;

		/*
		 * Load scene from file
		 */
		bool LoadSceneFromFile(CEDirectXCommandList& commandList, const std::wstring& fileName,
		                       const std::function<bool(float)>& loadingProgress);

		/*
		 * Load scene from string,
		 * Scene can be preloaded into a byte array and scan can be loaded from loaded byte array,
		 * @param scene, byte encoded scene file,
		 * @param format, format of scene file.
		 */
		bool LoadSceneFromString(CEDirectXCommandList& commandList, const std::string& sceneString,
		                         const std::string& format);

	private:
		void ImportScene(CEDirectXCommandList& commandList, const aiScene& scene, std::filesystem::path parentPath);
		void ImportMaterial(CEDirectXCommandList& commandList, const aiMaterial& material,
		                 std::filesystem::path parentPath);
		void ImportMesh(CEDirectXCommandList& commandList, const aiMaterial& mesh);
	};
}
