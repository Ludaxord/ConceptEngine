#pragma once
#include <d3d12.h>

#include "CEDXDeviceElement.h"

#include "../../../Main/RenderLayer/CEPipelineState.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::RenderLayer {
	class CEDXInputLayoutState : public Main::RenderLayer::CEInputLayoutState, public CEDXDeviceElement {

	};

	class CEDXDepthStencilState : public Main::RenderLayer::CEDepthStencilState, public CEDXDeviceElement {

	};

	class CEDXRasterizerState : public Main::RenderLayer::CERasterizerState, public CEDXDeviceElement {

	};

	class CEDXBlendState : public Main::RenderLayer::CEBlendState, public CEDXDeviceElement {

	};

	class CEDXGraphicsPipelineState : public Main::RenderLayer::CEGraphicsPipelineState, public CEDXDeviceElement {

	};

	class CEDXComputePipelineState : public Main::RenderLayer::CEComputePipelineState, public CEDXDeviceElement {

	};

	struct CERayTracingShaderIdentifier {
		char ShaderIdentifier[D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES];
	};

	class CEDXRayTracingPipelineState : public Main::RenderLayer::CERayTracingPipelineState, public CEDXDeviceElement {

	};
}
