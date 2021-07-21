#include "CEDXAftermath.h"

#include "CEDXDevice.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

CEDXAftermath Aftermath;

CEDXAftermath::CEDXAftermath(): AftermathCommandListContext(nullptr),
                                GPUCrashTracker() {
}

bool CEDXAftermath::CreateAMCommandList(ID3D12CommandList* D3DCommandList) {
	GFSDK_Aftermath_Result Result = GFSDK_Aftermath_DX12_CreateContextHandle(
		D3DCommandList, &AftermathCommandListContext);

	if (!GFSDK_Aftermath_SUCCEED(Result)) {
		return false;
	}

	return true;
}

bool CEDXAftermath::CreateCrashTracker() {
	if (!GPUCrashTracker.Create()) {
		return false;
	}

	return true;
}

bool CEDXAftermath::Create(CEDXDevice* Device) const {
	const uint32_t AftermathFlags =
		GFSDK_Aftermath_FeatureFlags_EnableMarkers |
		GFSDK_Aftermath_FeatureFlags_EnableResourceTracking |
		GFSDK_Aftermath_FeatureFlags_CallStackCapturing |
		GFSDK_Aftermath_FeatureFlags_GenerateShaderDebugInfo;

	GFSDK_Aftermath_Result Result = GFSDK_Aftermath_DX12_Initialize(GFSDK_Aftermath_Version_API, AftermathFlags,
	                                                                Device->GetDevice());
	if (!GFSDK_Aftermath_SUCCEED(Result)) {
		return false;
	}

	return true;
}
