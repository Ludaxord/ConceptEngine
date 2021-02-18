#pragma once
#include "CEScreen.h"

namespace ConceptEngineFramework::Game {
	class CEGame;

	class CEWindow : public CEScreen {

	public:
		void Create() override;
		void Destroy() override;
		void Show() override;
		void Hide() override;

	protected:
		friend CEGame;
		CEWindow(std::wstring windowName, HINSTANCE hInstance);
		~CEWindow() = default;
	private:

	};
}
