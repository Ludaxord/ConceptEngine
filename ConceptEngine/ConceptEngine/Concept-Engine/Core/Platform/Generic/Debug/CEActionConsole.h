#pragma once
#include <string>
#include <unordered_map>

#include "CEConsoleCommand.h"

#include "imgui.h"

#include "../../../Common/CEEvents.h"
#include "../../../Containers/CEStaticArray.h"
#include "../../../Containers/CEArray.h"

namespace ConceptEngine::Core::Platform::Generic::Debug {
	class CEActionConsole {

		struct CELine {
			CELine() = default;

			CELine(const std::string& string, ImVec4 color) : String(string), Color(color) {

			}

			std::string String;
			ImVec4 Color;
		};

		struct CECandidate {
			static constexpr float TextPadding = 20.0f;

			CECandidate() = default;

			CECandidate(const std::string& text, const std::string& postfix) : Text(text), Postfix(postfix) {
				TextSize = ImGui::CalcTextSize(Text.c_str());
				TextSize.x += TextPadding;
			}

			std::string Text;
			std::string Postfix;
			ImVec2 TextSize;
		};

	public:
		virtual ~CEActionConsole() = default;
		void Create();
		void RegisterCommand(const std::string& name, CEConsoleCommand* object);
		void RegisterVariable(const std::string& name, CEConsoleVariable* object);

		virtual void Update();

		CEConsoleCommand* FindCommand(const std::string& name);
		CEConsoleVariable* FindVariable(const std::string& name);

		void PrintMessage(const std::string& message);
		void PrintWarning(const std::string& message);
		void PrintError(const std::string& message);

		void ClearHistory();

	protected:
		virtual void OnKeyPressedEvent(const Common::CEKeyPressedEvent& Event);
	private:
		void DrawInterface();
		bool RegisterObject(const std::string& name, CEConsoleObject* variable);
		CEConsoleObject* FindConsoleObject(const std::string& name);
		int32 TextCallback(ImGuiInputTextCallbackData* data);
		void Execute(const std::string& cmdString);

		std::unordered_map<std::string, CEConsoleObject*> ConsoleObjects;
		std::string PopupSelectedText;
		Containers::CEArray<CECandidate> Candidates;
		int32 CandidatesIndex = -1;

		Containers::CEStaticArray<char, 256> TextBuffer;

		Containers::CEArray<CELine> Lines;

		Containers::CEArray<std::string> History;
		uint32 HistoryLength = 50;
		int32 HistoryIndex = -1;

		bool UpdateCursorPosition = false;
		bool CandidateSelectionChanged = false;
		bool ScrollDown = false;
		bool IsActive = false;
	};
}
