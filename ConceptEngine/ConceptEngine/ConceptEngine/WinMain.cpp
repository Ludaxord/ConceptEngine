#include <memory>

#define NOMINMAX
#include <Windows.h>

#include "../ConceptEngineFramework/CEFramework.h"

//Samples imports
#include "Playgrounds/DirectX12/Samples/BasicBox/CEDX12BoxPlayground.h"
#include "Playgrounds/DirectX12/Samples/TexBox/CEDX12CratePlayground.h"
#include "Playgrounds/DirectX12/Samples/InitDirect3D/CEDX12InitDirect3DPlayground.h"
#include "Playgrounds/DirectX12/Samples/BasicWaves/CEDX12LandscapePlayground.h"
#include "Playgrounds/DirectX12/Samples/LitShapes/CEDX12LitShapesPlayground.h"
#include "Playgrounds/DirectX12/Samples/LitWaves/CEDX12LitWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/BasicShapes/CEDX12ShapesPlayground.h"
#include "Playgrounds/DirectX12/Samples/BlendingWaves/CEDX12BlendingWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/BlurWaves/CEDX12BlurWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/SobelWaves/CEDX12SobelWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/StencilShapes/CEDX12StencilShapesPlayground.h"
#include "Playgrounds/DirectX12/Samples/TexShapes/CEDX12TexShapesPlayground.h"
#include "Playgrounds/DirectX12/Samples/TexWaves/CEDX12TexWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/TreesWaves/CEDX12TreesWavesPlayground.h"

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
	// Graphics::CEPlayground* playground = new CEDX12LitShapesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12LitWavesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12CratePlayground();
	// Graphics::CEPlayground* playground = new CEDX12TexShapesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12TexWavesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12BlendingWavesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12StencilShapesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12TreesWavesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12BlurWavesPlayground();
	Graphics::CEPlayground* playground = new CEDX12SobelWavesPlayground();

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
