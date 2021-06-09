#pragma once
#include "CECore.h"

namespace ConceptEngine::Core::Application {
	class CEGameCore : public CECore {
	public:
		CEGameCore(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CEGameCore(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		           Graphics::Main::Common::CEPlayground* playground);

		bool Create() override;
		void Run() override;
		bool Release() override;
	};
}
