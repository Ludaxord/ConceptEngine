#include "ConceptEngine.h"

#include "../../GameEngine/Shared/CEGame.h"
#include "../../GraphicsEngine/DirectX/Renderer/Direct3D/CEDevice.h"

using namespace Concept::Engine;
using namespace Concept::GameEngine;
using namespace Concept::GraphicsEngine::DirectX::Renderer::Direct3D;

ConceptEngine::ConceptEngine(HINSTANCE hInstance) {
#if defined(_DEBUG)
	CEDevice::EnableDebugLayer();
#endif
	CEGame::Create(hInstance);
}

int ConceptEngine::Run() {
	int returnCode = 0;
	{
		// auto playground = std::make_unique<Playground::CECubePlayground>(L"ConceptFramework Engine Editor (Cube Playground)", 1920, 1080);
		// auto playground = std::make_unique<Playground::CEBasicLightingPlayground>(L"ConceptFramework Engine Editor (Basic Lighting Playground)", 1920, 1080);
		// auto playground = std::make_unique<Playground::CEHDRPlayground>(L"ConceptFramework Engine Editor (HDR Playground)", 1920, 1080);
		// auto playground = std::make_unique<Playground::CEEditorPlayground>(L"ConceptFramework Engine Editor (Editor Playground)", 1920, 1080);
		// auto playground = std::make_unique<Playground::CERayTracingPlayground>(L"ConceptFramework Engine Editor (Ray Tracing Playground)", 1920, 1080);
		// returnCode = playground->Run();
	}
	// CEGame::Destroy();
	// atexit(&CEDevice::ReportLiveObjects);

	return returnCode;
}
