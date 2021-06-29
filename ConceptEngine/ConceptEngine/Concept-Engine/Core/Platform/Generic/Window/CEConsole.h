#pragma once
#include <string>

#include "CEScreen.h"

#include "../../../Common/CETypes.h"

namespace ConceptEngine::Core::Platform::Generic::Window {
	enum class CEConsoleColor : uint8 {
		Red = 0,
		Green = 1,
		Yellow = 2,
		White = 3,
		Blue = 4,
		Magenta = 5,
		Cyan = 6
	};
	
	class CEConsole : public CEScreen {
	public:
		CEConsole();
		~CEConsole();

		virtual void Update() = 0;

		virtual void Print(const std::string& message) = 0;
		virtual void Clear() = 0;
		virtual void SetTitle(const std::string& title) = 0;
		virtual void SetColor(CEConsoleColor color) = 0;
	};
}
