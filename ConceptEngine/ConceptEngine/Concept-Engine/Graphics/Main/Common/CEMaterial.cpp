#include "CEMaterial.h"

using namespace ConceptEngine::Graphics::Main::Common;
using namespace ConceptEngine::Graphics::Main::RenderLayer;

CEMaterial::CEMaterial(const CEMaterialProperties& Properties): AlbedoMap(), NormalMap(), RoughnessMap(), HeightMap(),
                                                                AOMap(), MetallicMap(), Properties(Properties),
                                                                MaterialBuffer() {
}

void CEMaterial::Create() {
}

void CEMaterial::BuildBuffer() {
}

void CEMaterial::SetMetallic(float metallic) {
	Properties.Metallic = metallic;
	MaterialBufferIsDirty = true;
}

void CEMaterial::SetRoughness(float roughness) {
	Properties.Roughness = roughness;
	MaterialBufferIsDirty = true;
}

void CEMaterial::SetAmbientOcclusion(float AO) {
	Properties.AO = AO;
	MaterialBufferIsDirty = true;
}

void CEMaterial::EnableHeightMap(bool enableHeightMap) {
	if (enableHeightMap) {
		Properties.EnableHeight = 1;
	}
	else {
		Properties.EnableHeight = 0;
	}
}

void CEMaterial::SetDebugName(const std::string& debugName) {
	DebugName = debugName;
}

CEShaderResourceView* const* CEMaterial::GetShaderResourceViews() const {
	ShaderResourceViews[0] = (AlbedoMap != nullptr) ? AlbedoMap->GetShaderResourceView() : nullptr;
	ShaderResourceViews[1] = (NormalMap != nullptr) ? NormalMap->GetShaderResourceView() : nullptr;
	ShaderResourceViews[2] = (RoughnessMap != nullptr) ? RoughnessMap->GetShaderResourceView() : nullptr;
	ShaderResourceViews[3] = (HeightMap != nullptr) ? HeightMap->GetShaderResourceView() : nullptr;
	ShaderResourceViews[4] = (MetallicMap != nullptr) ? MetallicMap->GetShaderResourceView() : nullptr;
	ShaderResourceViews[5] = (AOMap != nullptr) ? AOMap->GetShaderResourceView() : nullptr;
	ShaderResourceViews[6] = (AlphaMask != nullptr) ? AlphaMask->GetShaderResourceView() : nullptr;

	return ShaderResourceViews.Data();
}
