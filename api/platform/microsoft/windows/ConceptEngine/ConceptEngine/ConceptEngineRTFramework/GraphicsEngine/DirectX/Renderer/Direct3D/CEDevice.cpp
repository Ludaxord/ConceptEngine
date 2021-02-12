#include "CEDevice.h"

#include "../../../../Tools/Graphics/DirectX/CEHelper.h"

using namespace Concept::GraphicsEngine::DirectX::Renderer::Direct3D;

void CEDevice::EnableDebugLayer() {
	wrl::ComPtr<ID3D12Debug> debugInterface;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
}

void CEDevice::ReportLiveObjects() {
}
