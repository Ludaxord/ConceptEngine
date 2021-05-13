#pragma once
namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEPipelineState : public CEResource {
	};

	class CEComputePipelineState : public CEPipelineState {
	};

	class CEGraphicsPipelineState : public CEPipelineState {
	};

	class CERayTracingPipelineState : public CEPipelineState {

	};
}
