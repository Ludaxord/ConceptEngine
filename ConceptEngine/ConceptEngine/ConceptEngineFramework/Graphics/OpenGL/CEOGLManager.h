#pragma once
#include <memory>

#include "../CEGraphicsManager.h"

namespace ConceptEngineFramework::Graphics::OpenGL {
	class CEOGLManager : public CEGraphicsManager {

	public:
		void Create() override;
	protected:
		friend class CEGame;
		friend class std::default_delete<CEOGLManager>;

		CEOGLManager();
		~CEOGLManager() = default;

	};
}
