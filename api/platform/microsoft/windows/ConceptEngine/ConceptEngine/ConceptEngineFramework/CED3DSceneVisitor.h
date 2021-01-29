#pragma once
#include "CEVisitor.h"

namespace Concept::GraphicsEngine::Direct3D {
	class CECommandList;

	namespace Visitor {
		class CED3DSceneVisitor : public CEVisitor {
		public:
			/*
			 * Constructor for SceneVisitor
			 * @param commandList, commandList that is used to render meshes in scene
			 */
			CED3DSceneVisitor(CECommandList& commandList);

			/*
			 * Right now only override.
			 */

			virtual void Visit(CEScene& scene) override {
			};

			virtual void Visit(CESceneNode& scene) override {
			};

			virtual void Visit(CEMesh& scene) override;

		protected:
		private:
			CECommandList& m_commandList;
		};
	}
}
