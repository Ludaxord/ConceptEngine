#pragma once

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEDebugUI {
	public:
		CEDebugUI();
		virtual ~CEDebugUI();
		virtual bool Create() = 0;
	protected:
	private:
	};
}
