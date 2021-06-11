#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"
#include "../RenderLayer/CECommandList.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CELightProbeRenderer {
	public:
		CELightProbeRenderer() = default;
		virtual ~CELightProbeRenderer() = default;

		virtual bool Create(CELightSetup lightSetup, CEFrameResources& frameResources) = 0;
		void Release();

		virtual void RenderSkyLightProbe(RenderLayer::CECommandList& commandList, const CELightSetup& lightSetup,
		                         const CEFrameResources& resources) = 0;

	protected:
		CERef<RenderLayer::CEComputePipelineState> IrradianceGenPSO;
		CERef<RenderLayer::CEComputeShader> IrradianceGenShader;
		CERef<RenderLayer::CEComputePipelineState> SpecularIrradianceGenPSO;
		CERef<RenderLayer::CEComputeShader> SpecularIrradianceGenShader;
	};
}
