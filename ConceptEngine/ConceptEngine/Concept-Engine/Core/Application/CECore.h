#pragma once

#include "../../Graphics/Main/Common/API.h"
#include "../Compilers/CECompiler.h"
#include "../Platform/Generic/CEPlatform.h"
#include "../../Core/Platform/CEPlatformActions.h"
#include "../../Graphics/Main/Common/CEPlayground.h"
#include "../../Core/Platform/Generic/Callbacks/CEEngineController.h"
#include "../../Core/Platform/Generic/Debug/CETypedConsole.h"
#include "../../Core/Debug/CEProfiler.h"
#include "../../Core/Debug/CEDebug.h"
#include "../../Core/Threading/CETaskManager.h"
#include "../../Render/CERenderer.h"
#include "../../Graphics/DirectX12/CEDirectX12.h"
#include "../../Graphics/Vulkan/CEVulkan.h"
#include "../../Graphics/OpenGL/CEOpenGL.h"
#include "../../Graphics/Metal/CEMetal.h"
#include "../../Core/Platform/Android/CEAndroid.h"
#include "../../Core/Platform/Mac/CEMac.h"
#include "../../Core/Platform/Windows/CEWindows.h"
#include "../../Core/Platform/iOS/CEiOS.h"
#include "../../Core/Platform/Linux/CELinux.h"
#include "../../Core/Compilers/Cpp/CECppCompiler.h"
#include "../../Core/Compilers/CSharp/CECSCompiler.h"
#include "../../Core/Compilers/Python/CEPyCompiler.h"
#include "../../Core/Compilers/JS/CEJSCompiler.h"
#include "../../Core/Compilers/Schematics/CESchematicsCompiler.h"

namespace ConceptEngine::Core::Application {
	class CECore {
	public:
		CECore(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CECore(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		       Graphics::Main::Common::CEPlayground* playground);
		virtual ~CECore() = default;

		virtual bool Create();
		virtual void Run() = 0;
		virtual void Update(Time::CETimestamp DeltaTime) = 0;
		virtual bool Release() = 0;

		static Graphics::Main::CEGraphics* GetGraphics();
		static Compilers::CECompiler* GetCompiler();
		static Generic::Platform::CEPlatform* GetPlatform();
		static Graphics::Main::Common::CEPlayground* GetPlayground();

		inline static bool ShowConsole = false;
		inline static bool IsRunning = false;
		inline static bool EnableDebug = false;
	
	protected:
		static Graphics::Main::CEGraphics* Graphics;
		static Compilers::CECompiler* Compiler;
		static Generic::Platform::CEPlatform* Platform;

	private:
		Graphics::Main::CEGraphics* SelectGraphicsAPI(GraphicsAPI api);
		Compilers::CECompiler* SelectLanguageCompiler(Compilers::Language language);
		Generic::Platform::CEPlatform* SelectPlatform(Generic::Platform::Platform platform);
	};
}
