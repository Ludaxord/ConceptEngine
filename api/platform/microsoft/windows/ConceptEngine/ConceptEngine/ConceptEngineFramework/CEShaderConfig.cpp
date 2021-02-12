#include "CEShaderConfig.h"

ConceptFramework::GraphicsEngine::Direct3D::CEShaderConfig::CEShaderConfig(uint32_t maxAttributeSizeInBytes,
	uint32_t maxPayloadSizeInBytes) {
	shaderConfig.MaxAttributeSizeInBytes = maxAttributeSizeInBytes;
	shaderConfig.MaxPayloadSizeInBytes = maxPayloadSizeInBytes;

	subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
	subObject.pDesc = &shaderConfig;
}
