#pragma once


#include <Windows.h>
#include <fcntl.h>

#include <iostream>
#include <comdef.h>

#include <spdlog/async.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "Ws2_32.lib")

#include <gainput/gainput.h>



#include "CED3DCubeObject.h"
#include "CEOSTools.h"
#include "CEWindow.h"

#include "CEGame.h"

#include "CEDevice.h"
#include "CEEvents.h"
#include "CEScreen.h"
#include "CETimer.h"

using namespace Concept::GameEngine;
using namespace Concept::GraphicsEngine::Direct3D;

void OnUpdate(UpdateEventArgs& e);
void OnKeyPressed(KeyEventArgs& e);
void OnMouseWheel(MouseWheelEventArgs& e);
void OnMouseButtonPress(MouseButtonEventArgs& e);
void OnWindowResized(ResizeEventArgs& e);
void OnWindowClose(WindowCloseEventArgs& e);

inline std::shared_ptr<CEScreen> pGameWindow = nullptr;
inline std::shared_ptr<CEDevice> pDevice = nullptr;
inline std::shared_ptr<CESwapChain> pSwapChain = nullptr;
inline std::shared_ptr<CETexture> pDepthTexture = nullptr;
inline std::shared_ptr<CEVertexBuffer> pVertexBuffer = nullptr;
inline std::shared_ptr<CEIndexBuffer> pIndexBuffer = nullptr;
inline std::shared_ptr<CERootSignature> pRootSignature = nullptr;
inline std::shared_ptr<CEPipelineStateObject> pPipelineStateObject = nullptr;

inline Logger logger;

/*
 * Objects
 */
inline std::shared_ptr<Objects::CED3DCubeObject> pCube = nullptr;

namespace Concept {


	class ConceptEngine {
	public:
		ConceptEngine(HINSTANCE hInstance);

		virtual ~ConceptEngine() {
		}

		int Run();


	protected:
		int RunEngine();


	private:

		/** =========================================================================
		 *  ========== Deprecated - will be removed in upcoming versions ============
		 *  =========================================================================
		 */
	public:
		ConceptEngine(CEOSTools::CEGraphicsApiTypes graphicsApiType);
		ConceptEngine(int width, int height, const char* name);
		ConceptEngine(int width, int height, const char* name, CEOSTools::CEGraphicsApiTypes graphicsApiType);

		void Init();
		void MakeFrame();

		int RunGraphics();

		Logger CreateLogger(const std::string& name) const;

		static Logger GetLogger();

	protected:
		void InitSpdLog();
		static void CreateConsoleWindow();


	private:
		/**
		 * Global Engine objects variables:
		 */
		inline static Logger static_logger_ = nullptr;
		CEOSTools::CEGraphicsApiTypes apiType_ = CEOSTools::CEGraphicsApiTypes::direct3d12;
		std::unique_ptr<CEWindow> window_ = nullptr;
		CETimer timer_;
		Logger logger_;
	};

}
