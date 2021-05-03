#pragma once
#include "CECore.h"

namespace ConceptEngine::Core::Application {
	class CEGameCore : public CECore {
	public:
		CEGameCore(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CEGameCore(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform, Graphics::Main::Common::CEPlayground* playground);
		
		bool Init() override;
		int Run() override;
	};
}
