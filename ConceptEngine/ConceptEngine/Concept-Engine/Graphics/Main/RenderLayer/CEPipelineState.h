#pragma once
namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEPipelineState : public CEResource {
	};

	class CEDepthStencilState : public CEResource {
	};

	class CERasterizerState : public CEResource {
	};

	class CEBlendState : public CEResource {
	};

	class CEInputLayoutState : public CEResource {
	};

	class CEComputePipelineState : public CEPipelineState {
	};

	class CEGraphicsPipelineState : public CEPipelineState {
	};

	class CERayTracingPipelineState : public CEPipelineState {
	};

	struct CEDepthStencilStateCreateInfo {
	};

	struct CERasterizerStateCreateInfo {
	};

	struct CEBlendStateCreateInfo {
	};

	struct CEInputLayoutStateCreateInfo {
	};

	struct CEGraphicsPipelineStateCreateInfo {
	};

	struct CEComputePipelineStateCreateInfo {
	};

	struct CERayTracingPipelineStateCreateInfo {
	};
}
