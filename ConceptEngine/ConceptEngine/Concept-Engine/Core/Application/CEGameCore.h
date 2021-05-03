#pragma once
#include "CECore.h"

namespace ConceptEngine::Core::Application {
	class CEGameCore : public CECore {
	public:
		CEGameCore(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);

		bool Init() override;
		int Run() override;
	};
}
