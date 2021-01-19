#pragma once
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	class CEDirectXMesh;
	class CEDirectXSceneNode;
	class CEDirectXScene;
	namespace wrl = Microsoft::WRL;

	class CEDirectXVisitor {
	public:
		CEDirectXVisitor() = default;
		virtual ~CEDirectXVisitor() = default;

		virtual void Visit(CEDirectXScene& scene) = 0;
		virtual void Visit(CEDirectXSceneNode& scene) = 0;
		virtual void Visit(CEDirectXMesh& scene) = 0;
	};
}
