#pragma once
#include "../../Core/Application/Events.h"

#include "ConsoleVariable.h"
#include "ConsoleCommand.h"

#include <unordered_map>

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(push)
    #pragma warning(disable : 4100) // Disable unreferenced variable
#endif

#define INIT_CONSOLE_VARIABLE(Name, Variable) GConsole.RegisterVariable(Name, Variable)
#define INIT_CONSOLE_COMMAND(Name, Command)   GConsole.RegisterCommand(Name, Command)

class CEActionConsole
{
    struct Line
    {
        Line() = default;

        Line(const String& InString, ImVec4 InColor)
            : String(InString)
            , Color(InColor)
        {
        }

        String String;
        ImVec4 Color;
    };

    struct Candidate
    {
        static constexpr float TextPadding = 20.0f;

        Candidate() = default;

        Candidate(const String& InText, const String& InPostFix)
            : Text(InText)
            , PostFix(InPostFix)
        {
            TextSize = ImGui::CalcTextSize(Text.c_str());
            TextSize.x += TextPadding;
        }

        String Text;
        String PostFix;
        ImVec2 TextSize;
    };

public:
    void Create();
    void Update();

    void RegisterCommand(const String& Name, ConsoleCommand* Object);
    void RegisterVariable(const String& Name, ConsoleVariable* Variable);

    ConsoleCommand* FindCommand(const String& Name);
    ConsoleVariable* FindVariable(const String& Name);

    void PrintMessage(const String& Message);
    void PrintWarning(const String& Message);
    void PrintError(const String& Message);

    void ClearHistory();

private:
    void OnKeyPressedEvent(const KeyPressedEvent& Event);

    void DrawInterface();

    bool RegisterObject(const String& Name, ConsoleObject* Variable);

    ConsoleObject* FindConsoleObject(const String& Name);

    int32 TextCallback(ImGuiInputTextCallbackData* Data);

    void Execute(const String& CmdString);

private:
    std::unordered_map<String, ConsoleObject*> ConsoleObjects;

    String PopupSelectedText;

    CEArray<Candidate> Candidates;
    int32 CandidatesIndex = -1;

    CEStaticArray<char, 256> TextBuffer;

    CEArray<Line> Lines;

    CEArray<String> History;
    uint32 HistoryLength = 50;
    int32  HistoryIndex   = -1;

    bool UpdateCursorPosition      = false;
    bool CandidateSelectionChanged = false;
    bool ScrollDown                = false;
    bool IsActive                  = false;
};

extern CEActionConsole GConsole;

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(pop)
#endif