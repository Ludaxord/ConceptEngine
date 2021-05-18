#include "CEDXMaterial.h"

using namespace ConceptEngine::Graphics::DirectX12::Modules::Common;

void CEDXMaterial::SetAlbedo(float R, float G, float B) {
	SetAlbedo(DirectX::XMFLOAT3(R, G, B));
}

void CEDXMaterial::SetAlbedo(const DirectX::XMFLOAT3& albedo) {
	Properties.Albedo = albedo;
	MaterialBufferIsDirty = true;
}
