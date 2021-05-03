#pragma once

#include "../CEGameCore.h"

namespace ConceptEngine::Core::Application::Game {
	class CEGame final : public CEGameCore {
	public:
		CEGame(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CEGame(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		       Graphics::Main::Common::CEPlayground* playground);
		~CEGame() override;

		bool Init() override;
		int Run() override;
	};
}
