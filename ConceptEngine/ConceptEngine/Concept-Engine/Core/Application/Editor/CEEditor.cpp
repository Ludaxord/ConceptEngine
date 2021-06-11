#include "CEEditor.h"


#include "../../../Graphics/Main/Common/API.h"
#include "../../Compilers/CECompiler.h"
#include "../../Platform/Generic/CEPlatform.h"
#include "../../../Graphics/Main/Common/CEPlayground.h"
#include "../../../Core/Platform/Generic/Callbacks/CEEngineController.h"
#include "../../../Core/Platform/Generic/Debug/CETypedConsole.h"
#include "../../../Core/Debug/CEProfiler.h"
#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Threading/CETaskManager.h"
#include "../../../Render/CERenderer.h"
#include "../../../Graphics/DirectX12/CEDirectX12.h"
#include "../../../Core/Platform/Windows/CEWindows.h"

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
