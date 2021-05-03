#pragma once

#include "../CEGameCore.h"

namespace ConceptEngine::Core::GameDebug {
	class CEGameDebug final : public CEGameCore {
	public:
		CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language);
		~CEGameDebug() override;
		
		int Run() override;
		void Update() override;
		void Render() override;
	};
}
