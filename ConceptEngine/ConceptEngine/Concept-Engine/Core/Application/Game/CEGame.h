#pragma once

#include "../CEGameCore.h"

namespace ConceptEngine::Core::Application::Game {
	class CEGame final : public CEGameCore {
	public:
		CEGame(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CEGame(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		       Graphics::Main::Common::CEPlayground* playground);
		~CEGame() override;

		bool Create() override;
		void Update(Time::CETimestamp DeltaTime) override;
	};
}
