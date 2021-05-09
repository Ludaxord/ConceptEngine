#pragma once
#include "../../Generic/Window/CEConsole.h"
#include "../../../Threading/CEMutex.h"
#include "../../../Threading/CEScopedLock.h"
#include "../../../../Memory/CEMemory.h"

namespace ConceptEngine::Core::Platform::Windows::Window {
	class CEWindowsConsole final : public Generic::Window::CEConsole {
	public:
		CEWindowsConsole();
		~CEWindowsConsole() override;

		bool Create() override;
		void Update() override;
		void Print(const std::string& message) override;
		void Clear() override;
		void SetTitle(const std::string& title) override;
		void SetColor(Generic::Window::CEConsoleColor color) override;
	protected:
	private:
		HANDLE ConsoleHandle;
		CEMutex ConsoleMutex;
	};
}
