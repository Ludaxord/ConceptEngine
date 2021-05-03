#pragma once

#include "../CECore.h"

namespace ConceptEngine::Core::Application::Editor {
	class CEEditor final : public CECore {
	public:
		CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		       Graphics::Main::Common::CEPlayground* playground);
		~CEEditor() override;

		bool Init() override;
		int Run() override;
	};
}
