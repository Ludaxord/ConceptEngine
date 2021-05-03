#pragma once

#include "../CEGameCore.h"

namespace ConceptEngine::Core::Application::Game {
	class CEGame final : public CEGameCore {
	public:
		CEGame(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		~CEGame() override;

		bool Init() override;
		int Run() override;
		void Update() override;
		void Render() override;
	};
}
