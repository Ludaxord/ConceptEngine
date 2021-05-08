#pragma once
#include <string>

#include "CEScreen.h"

#include "../../../Common/CETypes.h"

namespace ConceptEngine::Core::Platform::Generic::Window {
	enum class CEConsoleColor : uint8 {
		Red = 0,
		Green = 0,
		Yellow = 0,
		White = 0
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
