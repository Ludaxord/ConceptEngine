#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::OpenGL {
	class CEOpenGL : public Main::CEGraphics {
	public:
		CEOpenGL();
		virtual ~CEOpenGL();
		
		void Create() override;
		void CreateManagers() override;
	};
}
