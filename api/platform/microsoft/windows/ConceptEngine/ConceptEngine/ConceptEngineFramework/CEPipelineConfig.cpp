#include "CEPipelineConfig.h"

ConceptFramework::GraphicsEngine::Direct3D::CEPipelineConfig::CEPipelineConfig(uint32_t maxTraceRecursionDepth) {
	pipelineConfig.MaxTraceRecursionDepth = maxTraceRecursionDepth;

	subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
	subObject.pDesc = &pipelineConfig;
}
