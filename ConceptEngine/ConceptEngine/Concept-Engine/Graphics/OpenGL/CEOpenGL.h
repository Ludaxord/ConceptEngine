#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::OpenGL {
	class CEOpenGL final : public Main::CEGraphics {
	public:
		CEOpenGL();
		~CEOpenGL() override;
		
		void Create() override;
		void CreateManagers() override;
		void CreateGraphicsManager() override;
		
		void Update() override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
	};
}
