#include "CEGameCore.h"

#include "../../Graphics/Main/Common/API.h"
#include "../Compilers/CECompiler.h"
#include "../Platform/Generic/CEPlatform.h"
#include "../../Graphics/Main/Common/CEPlayground.h"
#include "../../Core/Platform/Generic/Callbacks/CEEngineController.h"
#include "../../Core/Platform/Generic/Debug/CETypedConsole.h"
#include "../../Core/Debug/CEProfiler.h"
#include "../../Core/Debug/CEDebug.h"
#include "../../Core/Threading/CETaskManager.h"
#include "../../Render/CERenderer.h"
#include "../../Graphics/DirectX12/CEDirectX12.h"
#include "../../Core/Platform/Windows/CEWindows.h"

using namespace ConceptEngine::Core;

Application::CEGameCore::CEGameCore(GraphicsAPI api,
                                    Compilers::Language language,
                                    Generic::Platform::Platform platform): CECore(api, language, platform) {
}

Application::CEGameCore::CEGameCore(GraphicsAPI api, Compilers::Language language,
                                    Generic::Platform::Platform platform,
                                    Graphics::Main::Common::CEPlayground* playground): CECore(
	api, language, platform, playground) {
}

bool Application::CEGameCore::Create() {
	if (!GPlatform->CreateSystemWindow()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!GPlatform->CreateCursors()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!CECore::Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

void Application::CEGameCore::Run() {
	Time::CETimer Timer;
	while (IsRunning) {
		Timer.Update();
		Update(Timer.GetDeltaTime());
	}
}

bool Application::CEGameCore::Release() {
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
