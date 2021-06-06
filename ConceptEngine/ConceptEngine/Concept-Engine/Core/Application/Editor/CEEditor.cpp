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
		Debug::CEDebug::DebugBreak();
		return false;
	}

	return true;
}

int CEEditor::Run() {
	Timer = Time::CETimer();
	return 0;
}

void CEEditor::Update(Time::CETimestamp DeltaTime) {
	Platform->Update();
	Graphics->Update(DeltaTime, [] {
		Debug::CEProfiler::Update();
	});
}

bool CEEditor::Release() {
		TRACE_FUNCTION_SCOPE();

	CommandListExecutor.WaitForGPU();

	CastTextureManager()->Release();

	if (!GetPlayground()->Release()) {
		return false;
	}

	if (Platform::Generic::Callbacks::EngineController.Release()) {
		Platform->SetCallbacks(nullptr);
	}
	else {
		return false;
	}

	GetDebugUI()->Release();

	Graphics->Destroy();

	Threading::CETaskManager::Get().Release();

	if (!Platform->Release()) {
		return false;
	}

	GTypedConsole.Release();

	return true;
}
