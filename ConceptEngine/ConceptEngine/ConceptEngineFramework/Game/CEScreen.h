#pragma once

#include "../CEHeaderLibs.h"


namespace ConceptEngineFramework::Game {
	interface CEScreen {
	protected:
		~CEScreen() = default;
	public:
		virtual void Create() = 0;
		virtual void Destroy() = 0;
		virtual void Show() = 0;
		virtual void Hide() = 0;

		virtual std::wstring GetName() = 0;

	};
}
