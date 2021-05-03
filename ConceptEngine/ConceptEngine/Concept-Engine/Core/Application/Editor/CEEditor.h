#pragma once

#include "../CECore.h"

namespace ConceptEngine::Core::Application::Editor {
	class CEEditor final : public CECore {
	public:
		CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language, Platform::Platform platform);
		~CEEditor() override;
		
		int Run() override;
		void Update() override;
		void Render() override;
	};
}
