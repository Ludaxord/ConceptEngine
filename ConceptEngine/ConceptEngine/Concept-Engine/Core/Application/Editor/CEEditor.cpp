#include "CEEditor.h"

#include "../../Debug/CEProfiler.h"

#include "../../Debug/CEDebug.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"
#include "../../Platform/Generic/Callbacks/CEEngineController.h"
#include "../../Threading/CETaskManager.h"

#include "../../../Core/Platform/Generic/Debug/CETypedConsole.h"
using namespace ConceptEngine::Core::Application::Editor;

//TODO: Cast with QT framework.

CEEditor::CEEditor(GraphicsAPI api, Compilers::Language language,
                   Generic::Platform::Platform platform): CECore(api, language, platform) {
	EnableDebug = true;
}

CEEditor::CEEditor(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
                   Graphics::Main::Common::CEPlayground* playground): CECore(api, language, platform, playground) {
}

CEEditor::~CEEditor() {
}

bool CEEditor::Create() {
	if (!CECore::Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

void CEEditor::Run() {
	Timer = Time::CETimer();
}

void CEEditor::Update(Time::CETimestamp DeltaTime) {
	GPlatform->Update();
	GGraphics->Update(DeltaTime, [] {
		Debug::CEProfiler::Update();
	});
}

bool CEEditor::Release() {
		TRACE_FUNCTION_SCOPE();

	CommandListExecutor.WaitForGPU();

	CastTextureManager()->Release();

	if (!GPlayground->Release()) {
		return false;
	}

	if (EngineController.Release()) {
		GPlatform->SetCallbacks(nullptr);
	}
	else {
		return false;
	}

	GetDebugUI()->Release();

	GGraphics->Destroy();

	Threading::CETaskManager::Get().Release();

	if (!GPlatform->Release()) {
		return false;
	}

	GTypedConsole.Release();

	return true;
}
