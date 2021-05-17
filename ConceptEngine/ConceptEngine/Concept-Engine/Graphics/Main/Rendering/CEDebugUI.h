#pragma once

namespace ConceptEngine::Graphics::Main::Rendering {
	struct ImGuiImage {
		ImGuiImage() = default;
		
	};

	class CEDebugUI {
	public:
		CEDebugUI();
		virtual ~CEDebugUI();

		typedef void (*UIDrawFunc)();

		virtual bool Create() = 0;
		virtual void DrawUI(UIDrawFunc DrawFunc);
	protected:
	private:
	};
}
