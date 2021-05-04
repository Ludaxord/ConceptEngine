#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::OpenGL {
	class CEOpenGL final : public Main::CEGraphics {
	public:
		CEOpenGL();
		~CEOpenGL() override;
		
		bool Create() override;
		bool CreateManagers() override;
		bool CreateGraphicsManager() override;
		bool CreateTextureManager() override;
		
		void Update() override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
	};
}
