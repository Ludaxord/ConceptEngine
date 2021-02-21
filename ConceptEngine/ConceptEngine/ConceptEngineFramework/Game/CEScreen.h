#pragma once

#include "../CEHeaderLibs.h"


namespace ConceptEngineFramework::Game {
	interface CEScreen {
	public:
		virtual void Create() = 0;
		virtual void Destroy() = 0;
		virtual void Show() = 0;
		virtual void Hide() = 0;

		virtual std::wstring GetName() = 0;
		virtual void SetName(std::wstring name) = 0;
	protected:
		~CEScreen() = default;
	};
}
