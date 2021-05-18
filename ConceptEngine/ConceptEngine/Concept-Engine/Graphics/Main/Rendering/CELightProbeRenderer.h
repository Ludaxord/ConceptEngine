#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"
#include "../RenderLayer/CECommandList.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CELightProbeRenderer {
	public:
		CELightProbeRenderer() = default;
		~CELightProbeRenderer() = default;

		bool Create(CELightSetup lightSetup, CEFrameResources& frameResources);
		void Release();

		void RenderSkyLightProbe(RenderLayer::CECommandList& commandList, const CELightSetup& lightSetup,
		                         const CEFrameResources& resources);

	private:
		bool CreateSkyLightResources(CELightSetup& lightSetup);

		Core::Common::CERef<RenderLayer::CEComputePipelineState> IrradianceGenPSO;
		Core::Common::CERef<RenderLayer::CEComputeShader> IrradianceGenShader;
		Core::Common::CERef<RenderLayer::CEComputePipelineState> SpecularIrradianceGenPSO;
		Core::Common::CERef<RenderLayer::CEComputeShader> SpecularIrradianceGenShader;
	};
}
