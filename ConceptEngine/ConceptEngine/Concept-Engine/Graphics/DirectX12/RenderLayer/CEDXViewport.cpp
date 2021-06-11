#include "CEDXViewport.h"

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::CEDXViewport(CEDXDevice* device,
                                                                            CEDXCommandContext* commandContext,
                                                                            HWND hwnd, CEFormat format, uint32 width,
                                                                            uint32 height) : CEViewport(format, width,
	height), CEDXDeviceElement(device) {
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::~CEDXViewport() {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::Create() {
	return false;
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::Resize(uint32 width, uint32 height) {
	return false;
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::Present(bool verticalSync) {
	return false;
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::SetName(const std::string& name) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::RetrieveBackBuffers() {
	return false;
}
