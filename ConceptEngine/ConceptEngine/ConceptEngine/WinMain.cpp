#include <memory>

#define NOMINMAX
#include <Windows.h>

#include "../ConceptEngineFramework/CEFramework.h"

//Samples imports
#include "../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "Playgrounds/DirectX12/Samples/BasicBox/CEDX12BoxPlayground.h"
#include "Playgrounds/DirectX12/Samples/TexBox/CEDX12CratePlayground.h"
#include "Playgrounds/DirectX12/Samples/InitDirect3D/CEDX12InitDirect3DPlayground.h"
#include "Playgrounds/DirectX12/Samples/BasicWaves/CEDX12LandscapePlayground.h"
#include "Playgrounds/DirectX12/Samples/LitShapes/CEDX12LitShapesPlayground.h"
#include "Playgrounds/DirectX12/Samples/LitWaves/CEDX12LitWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/BasicShapes/CEDX12ShapesPlayground.h"
#include "Playgrounds/DirectX12/Samples/BezierTessellationWaves/CEDX12BezierTessellationWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/BlendingWaves/CEDX12BlendingWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/BlurWaves/CEDX12BlurWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/ComputeWaves/CEDX12ComputeWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/CubeMap/CEDX12CubeMapPlayground.h"
#include "Playgrounds/DirectX12/Samples/DynamicCube/CEDX12DynamicCubePlayground.h"
#include "Playgrounds/DirectX12/Samples/FPPCamera/CEDX12FPPCameraPlayground.h"
#include "Playgrounds/DirectX12/Samples/Instancing/CEDX12InstancingPlayground.h"
#include "Playgrounds/DirectX12/Samples/NormalMap/CEDX12NormalMapPlayground.h"
#include "Playgrounds/DirectX12/Samples/Picking/CEDX12PickingPlayground.h"
#include "Playgrounds/DirectX12/Samples/Shadows/CEDX12ShadowsPlayground.h"
#include "Playgrounds/DirectX12/Samples/SobelWaves/CEDX12SobelWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/SSAO/CEDX12SSAOPlayground.h"
#include "Playgrounds/DirectX12/Samples/StencilShapes/CEDX12StencilShapesPlayground.h"
#include "Playgrounds/DirectX12/Samples/TessellationWaves/CEDX12TessellationWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/TexShapes/CEDX12TexShapesPlayground.h"
#include "Playgrounds/DirectX12/Samples/TexWaves/CEDX12TexWavesPlayground.h"
#include "Playgrounds/DirectX12/Samples/TreesWaves/CEDX12TreesWavesPlayground.h"

using namespace ConceptEngineFramework;
using namespace ConceptEngine::Playgrounds::DirectX12;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	CEDX12Manager::RTVCount = CEDX12Manager::BufferCount + 1;
	CEDX12Manager::RTVCount = CEDX12Manager::BufferCount + 6;
	CEDX12Manager::DSVCount = 2;
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
	// Graphics::CEPlayground* playground = new CEDX12SobelWavesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12ComputeWavesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12TessellationWavesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12BezierTessellationWavesPlayground();
	// Graphics::CEPlayground* playground = new CEDX12FPPCameraPlayground();
	// Graphics::CEPlayground* playground = new CEDX12InstancingPlayground(); 
	// Graphics::CEPlayground* playground = new CEDX12PickingPlayground(); 
	// Graphics::CEPlayground* playground = new CEDX12CubeMapPlayground();
	// Graphics::CEPlayground* playground = new CEDX12DynamicCubePlayground(); 
	// Graphics::CEPlayground* playground = new CEDX12NormalMapPlayground(); 
	Graphics::CEPlayground* playground = new CEDX12ShadowsPlayground(); 
	// Graphics::CEPlayground* playground = new CEDX12SSAOPlayground(); 

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
