#pragma once

#include "../CECore.h"

namespace ConceptEngine::Core::Editor {
	class CEEditor : public CECore {
	public:
		CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language);
		~CEEditor() override;
		int Run() override;
		void Update() override;
		void Render() override;
	};
}
