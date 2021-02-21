#include <memory>
#include <Windows.h>
#include "../ConceptEngineFramework/CEFramework.h"
#include "Playgrounds/DirectX12/Box/CEDX12BoxPlayground.h"
#include "Playgrounds/DirectX12/InitDirect3D/CEDX12InitDirect3DPlayground.h"

using namespace ConceptEngineFramework;
using namespace ConceptEngine::Playgrounds::DirectX12;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Graphics::CEPlayground* playground = new CEDX12InitDirect3DPlayground();
	// Graphics::CEPlayground* playground = new CEDX12BoxPlayground();
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
