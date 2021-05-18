#pragma once
#include <string>

#include "CERendering.h"

#include "../../../Core/Containers/CEArray.h"
#include "../../../Core/Common/CETypes.h"

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

	enum class CEInputClassification { Vertex = 0, Instance = 1 };

	struct CEInputElement {
		std::string Semantic = "";
		uint32 SemanticIndex = 0;
		CEFormat Format = CEFormat::Unknown;
		uint32 InputSlot = 0;
		uint32 ByteOffset = 0;
		CEInputClassification InputClassification = CEInputClassification::Vertex;
		uint32 InstanceStepRate = 0;
	};

	struct CEInputLayoutStateCreateInfo {
		CEInputLayoutStateCreateInfo() = default;

		CEInputLayoutStateCreateInfo(const Core::Containers::CEArray<CEInputElement>& elements): Elements(elements) {
		}

		CEInputLayoutStateCreateInfo(std::initializer_list<CEInputElement> list) : Elements(list) {

		}

		Core::Containers::CEArray<CEInputElement> Elements;
	};

	struct CEGraphicsPipelineStateCreateInfo {
	};

	struct CEComputePipelineStateCreateInfo {
	};

	struct CERayTracingPipelineStateCreateInfo {
	};
}
