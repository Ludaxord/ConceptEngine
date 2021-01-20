#pragma once
#include <wrl.h>

namespace Concept::GraphicsEngine::Direct3D {
	class CEMesh;
	class CESceneNode;
	class CEScene;
	namespace wrl = Microsoft::WRL;

	class CEVisitor {
	public:
		CEVisitor() = default;
		virtual ~CEVisitor() = default;

		virtual void Visit(CEScene& scene) = 0;
		virtual void Visit(CESceneNode& scene) = 0;
		virtual void Visit(CEMesh& scene) = 0;
	};
}
