#pragma once

#include "../CEGameCore.h"

namespace ConceptEngine::Core::Application::GameDebug {
	class CEGameDebug final : public CEGameCore {
	public:
		CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language,
		            Generic::Platform::Platform platform);
		~CEGameDebug() override;

		bool Init() override;
		int Run() override;
	};
}
