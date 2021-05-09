#include "CEActionConsole.h"

#include "CETypedConsole.h"
#include "../Callbacks/CEEngineController.h"
#include "../../../../Graphics/Main/CEGraphics.h"

#include "../../../Log/CELog.h"

using namespace ConceptEngine::Core::Platform::Generic::Debug;

CETypedConsole GTypedConsole;
CEConsoleCommand GClearHistory;

void CEActionConsole::Create() {
	GClearHistory.OnExecute.AddObject(this, &CEActionConsole::ClearHistory);
	INIT_CONSOLE_COMMAND("ClearHistory", &GClearHistory);
	Callbacks::EngineController.OnKeyPressedEvent.AddObject(this, &CEActionConsole::OnKeyPressedEvent);
}

void CEActionConsole::RegisterCommand(const std::string& name,
                                      CEConsoleCommand* object) {
}

void CEActionConsole::RegisterVariable(const std::string& name, CEConsoleVariable* object) {
}

void CEActionConsole::Update() {
	if (IsActive) {
		Graphics::Main::CEGraphics::GetDebugUI()->DrawUI([]() {
				GTypedConsole.DrawInterface();
			}
		);
	}
}

CEConsoleCommand* CEActionConsole::FindCommand(const std::string& name) {
	CEConsoleObject* consoleObject = FindConsoleObject(name);
	if (!consoleObject) {
		CE_LOG_ERROR("Could not find Console Command of name: '" + name + '\'');
		return nullptr;
	}

	CEConsoleCommand* consoleCommand = consoleObject->AsCommand();
	if (!consoleCommand) {
		CE_LOG_ERROR('\''+ name + "' Is Not a Console Command");
		return nullptr;
	}

	return consoleCommand;
}

CEConsoleVariable* CEActionConsole::FindVariable(const std::string& name) {
	CEConsoleObject* consoleObject = FindConsoleObject(name);
	if (!consoleObject) {
		CE_LOG_ERROR("Could not find Console Variable of name: '" + name + '\'');
		return nullptr;
	}

	CEConsoleVariable* consoleVariable = consoleObject->AsVariable();
	if (!consoleVariable) {
		CE_LOG_ERROR('\'' + name "' Is not a Console Variable");
		return nullptr;
	}

	return consoleVariable;
}

void CEActionConsole::PrintMessage(const std::string& message) {
}

void CEActionConsole::PrintWarning(const std::string& message) {
}

void CEActionConsole::PrintError(const std::string& message) {
}

void CEActionConsole::ClearHistory() {
}

void CEActionConsole::OnKeyPressedEvent(const Common::CEKeyPressedEvent& Event) {
}

void CEActionConsole::DrawInterface() {
}

bool CEActionConsole::RegisterObject(const std::string& name, CEConsoleObject* variable) {
	if (const auto existingObject = ConsoleObjects.find(name); existingObject == ConsoleObjects.end()) {
		ConsoleObjects.insert(std::make_pair(name, variable));
		return true;
	}

	return false;
}

CEConsoleObject* CEActionConsole::FindConsoleObject(const std::string& name) {
	if (const auto existingObject = ConsoleObjects.find(name); existingObject != ConsoleObjects.end()) {
		return existingObject->second;
	}

	return nullptr;
}

int32 CEActionConsole::TextCallback(ImGuiInputTextCallbackData* data) {
	//TODO: Implement

	return 0;
}

void CEActionConsole::Execute(const std::string& cmdString) {
}
