#include <memory>
#include <Windows.h>
#include "../ConceptEngineFramework/CEFramework.h"

//Samples imports
#include "Playgrounds/DirectX12/Samples/Box/CEDX12BoxPlayground.h"
#include "Playgrounds/DirectX12/Samples/InitDirect3D/CEDX12InitDirect3DPlayground.h"
#include "Playgrounds/DirectX12/Samples/Landscape/CEDX12LandscapePlayground.h"
#include "Playgrounds/DirectX12/Samples/LitShapes/CEDX12LitShapesPlayground.h"
#include "Playgrounds/DirectX12/Samples/Shapes/CEDX12ShapesPlayground.h"

using namespace ConceptEngineFramework;
using namespace ConceptEngine::Playgrounds::DirectX12;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	/**
	 * Playgrounds
	 */
	// Graphics::CEPlayground* playground = new CEDX12InitDirect3DPlayground();
	// Graphics::CEPlayground* playground = new CEDX12BoxPlayground();
	// Graphics::CEPlayground* playground = new CEDX12LandscapePlayground();
	// Graphics::CEPlayground* playground = new CEDX12ShapesPlayground();
	Graphics::CEPlayground* playground = new CEDX12LitShapesPlayground();

	const auto framework = std::make_shared<CEFramework>(L"ConceptEngine",
	                                                     hInstance,
	                                                     lpCmdLine,
	                                                     nCmdShow,
	                                                     1920,
	                                                     1080,
	                                                     Graphics::API::DirectX12_API,
	                                                     playground);
	return framework->Run();
}
