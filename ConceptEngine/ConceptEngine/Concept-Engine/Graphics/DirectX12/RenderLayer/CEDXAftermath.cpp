#include "CEDXAftermath.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

CEDXAftermath::CEDXAftermath(CEDXDevice* Device) : CEDXDeviceElement(Device), AftermathCommandListContext(nullptr),
                                                   GPUCrashTracker() {
}

//TODO: Think about splitting Initialize of Aftermath and Creating Context Handle
void CEDXAftermath::Create(CEDXCommandContext& commandContext) {
	GPUCrashTracker.Create();

	const uint32_t AftermathFlags =
		GFSDK_Aftermath_FeatureFlags_EnableMarkers |
		GFSDK_Aftermath_FeatureFlags_EnableResourceTracking |
		GFSDK_Aftermath_FeatureFlags_CallStackCapturing |
		GFSDK_Aftermath_FeatureFlags_GenerateShaderDebugInfo;

	AFTERMATH_CHECK_ERROR(
		GFSDK_Aftermath_DX12_Initialize(
			GFSDK_Aftermath_Version_API,
			AftermathFlags,
			GetDevice()->GetDevice()
		)
	);

	AFTERMATH_CHECK_ERROR(
		GFSDK_Aftermath_DX12_CreateContextHandle(
			commandContext.GetCommandList().GetCommandList(), &
			AftermathCommandListContext
		)
	);
}
