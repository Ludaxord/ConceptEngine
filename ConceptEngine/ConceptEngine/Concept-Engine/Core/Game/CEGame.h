#pragma once

#include "../CEGameCore.h"

namespace ConceptEngine::Core::Game {
	class CEGame : public CEGameCore {
	public:
		CEGame(Graphics::Main::GraphicsAPI api, Compilers::Language language);
		~CEGame() override;

		int Run() override;
		void Update() override;
		void Render() override;
	};
}
