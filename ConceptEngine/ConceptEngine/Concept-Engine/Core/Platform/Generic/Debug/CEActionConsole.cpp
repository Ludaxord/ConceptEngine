#include "CEActionConsole.h"

#include <regex>

#include "CEConsoleVariable.h"
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
	if (!RegisterObject(name, object)) {
		CE_LOG_WARNING("Console Command '" + name + "' os already registered")
	}
}

void CEActionConsole::RegisterVariable(const std::string& name, CEConsoleVariable* object) {
	if (!RegisterObject(name, object)) {
		CE_LOG_WARNING("Console Variable '" + name + "' os already registered")
	}
}

void CEActionConsole::Update() {
	if (IsActive) {
		Graphics::Main::CEGraphics::GetDebugUI()->DrawUI([]() {
				GTypedConsole.DrawInterface();
			}
		);
	}
}

void CEActionConsole::Release() {
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
	Lines.EmplaceBack(message, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
}

void CEActionConsole::PrintWarning(const std::string& message) {
	Lines.EmplaceBack(message, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
}

void CEActionConsole::PrintError(const std::string& message) {
	Lines.EmplaceBack(message, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
}

void CEActionConsole::ClearHistory() {
	History.Clear();
	HistoryIndex = -1;
}

void CEActionConsole::OnKeyPressedEvent(const Common::CEKeyPressedEvent& Event) {
	if (!Event.IsRepeat && Event.Key == Input::CEKey::Key_Tab) {
		IsActive = !IsActive;
	}
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
	if (UpdateCursorPosition) {
		data->CursorPos = int32(PopupSelectedText.length());
		PopupSelectedText.clear();
		UpdateCursorPosition = false;
	}

	switch (data->EventFlag) {
	case ImGuiInputTextFlags_CallbackEdit: {
		const char* wordEnd = data->Buf + data->CursorPos;
		const char* wordStart = wordEnd;
		while (wordStart > data->Buf) {
			const char c = wordStart[-1];
			if (c == ' ' || c == '\t' || c == ',' || c == ';') {
				break;
			}

			wordStart--;
		}

		Candidates.Clear();
		CandidateSelectionChanged = true;
		CandidatesIndex = -1;

		const int32 wordLength = static_cast<int32>(wordEnd - wordStart);
		if (wordLength <= 0) {
			break;
		}

		for (const std::pair<std::string, CEConsoleObject*>& object : ConsoleObjects) {
			if (wordLength <= object.first.size()) {
				const char* command = object.first.c_str();
				int32 d = -1;
				int32 n = wordLength;

				const char* cmdIt = command;
				const char* wordIt = wordStart;
				while (n > 0 && (d = toupper(*wordIt) - toupper(*cmdIt)) == 0) {
					cmdIt++;
					wordIt++;
					n--;
				}

				if (d == 0) {
					CEConsoleObject* consoleObject = object.second;
					Assert(consoleObject != nullptr);

					if (consoleObject->AsCommand()) {
						Candidates.EmplaceBack(object.first, "[CMD]");
					}
					else {
						CEConsoleVariable* variable = consoleObject->AsVariable();
						if (variable->IsBool()) {
							Candidates.EmplaceBack(object.first, "= " + variable->GetString() + " [Boolean]");
						}
						else if (variable->IsInt()) {
							Candidates.EmplaceBack(object.first, "= " + variable->GetString() + " [Integer]");
						}
						else if (variable->IsFloat()) {
							Candidates.EmplaceBack(object.first, "= " + variable->GetString() + " [Float]");
						}
						else if (variable->IsString()) {
							Candidates.EmplaceBack(object.first, "= " + variable->GetString() + " [String]");
						}
					}
				}
			}
		}
		break;
	}
	case ImGuiInputTextFlags_CallbackCompletion: {
		const char* wordEnd = data->Buf + data->CursorPos;
		const char* wordStart = wordEnd;
		if (data->BufTextLen > 0) {
			while (wordStart > data->Buf) {
				const char c = wordStart[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';') {
					break;
				}
				wordStart--;
			}
		}

		const int32 wordLength = static_cast<int32>(wordEnd - wordStart);
		if (wordLength > 0) {
			if (Candidates.Size() == 1) {
				const int32 position = static_cast<int32>(wordStart - data->Buf);
				const int32 count = wordLength;
				data->DeleteChars(position, count);
				data->InsertChars(data->CursorPos, Candidates[0].Text.c_str());

				CandidatesIndex = -1;
				CandidateSelectionChanged = true;
				Candidates.Clear();
			}
			else if (!Candidates.IsEmpty() && CandidatesIndex != -1) {
				const int32 position = static_cast<int32>(wordStart - data->Buf);
				const int32 count = wordLength;
				data->DeleteChars(position, count);
				data->InsertChars(data->CursorPos, PopupSelectedText.c_str());

				PopupSelectedText = "";

				Candidates.Clear();
				CandidatesIndex = -1;
				CandidateSelectionChanged = true;
			}
		}

		break;
	}
	case ImGuiInputTextFlags_CallbackHistory: {
		if (Candidates.IsEmpty()) {
			const int32 prevHistoryIndex = HistoryIndex;
			if (data->EventKey == ImGuiKey_UpArrow) {
				if (HistoryIndex == -1) {
					HistoryIndex = History.Size() - 1;
				}
				else if (HistoryIndex > 0) {
					HistoryIndex--;
				}
			}
			else if (data->EventKey == ImGuiKey_DownArrow) {
				if (HistoryIndex != -1) {
					HistoryIndex++;
					if (HistoryIndex >= static_cast<int32>(History.Size())) {
						HistoryIndex = -1;
					}
				}
			}

			if (prevHistoryIndex != HistoryIndex) {
				const char* historyString = (HistoryIndex >= 0) ? History[HistoryIndex].c_str() : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, historyString);
			}
		}
		else {
			if (data->EventKey == ImGuiKey_UpArrow) {
				CandidateSelectionChanged = true;
				if (CandidatesIndex <= 0) {
					CandidatesIndex = Candidates.Size() - 1;
				}
				else {
					CandidatesIndex--;
				}
			}
			else if (data->EventKey == ImGuiKey_DownArrow) {
				CandidateSelectionChanged = true;
				if (CandidatesIndex >= int32(Candidates.Size()) - 1) {
					CandidatesIndex = 0;
				}
				else {
					CandidatesIndex++;
				}
			}
		}
		break;
	}
	}

	return 0;
}

void CEActionConsole::Execute(const std::string& cmdString) {
	PrintMessage(cmdString);
	History.EmplaceBack(cmdString);
	if (History.Size() > HistoryLength) {
		History.Erase(History.Begin());
	}

	size_t position = cmdString.find_first_of(" ");
	if (position == std::string::npos) {
		CEConsoleCommand* command = FindCommand(cmdString);
		if (!command) {
			const std::string message = "'" + cmdString + "' is not a registered command";
			PrintError(message);
		}
		else {
			command->Execute();
		}
	}
	else {
		std::string variableName(cmdString.c_str(), position);
		CEConsoleVariable* variable = FindVariable(variableName);
		if (!variable) {
			PrintError("'" + cmdString + "' is not a registered variable");
			return;
		}

		position++;
		std::string value(cmdString.c_str() + position, cmdString.length() - position);
		if (std::regex_match(value, std::regex("[-]?[0-9]+"))) {
			variable->SetString(value);
		}
		else if (std::regex_match(value, std::regex("[-]?[0-9]*[.][0-9]+")) && variable->IsFloat()) {
			variable->SetString(value);
		}
		else if (std::regex_match(value, std::regex("(false)|(true)")) && variable->IsBool()) {
			variable->SetString(value);
		}
		else {
			if (variable->IsString()) {
				variable->SetString(value);
			}
			else {
				const std::string message = "'" + value + "' is invalid value for '" + variableName + "'";
				PrintError(message);
			}
		}
	}
}
