#include "CEDXAftermath.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

//TODO: Add Device, (Maybe make it DeviceElement extension), Add CommandList 
CEDXAftermath::CEDXAftermath(): AftermathCommandListContext(nullptr), GPUCrashTracker() {
}

CEDXAftermath::~CEDXAftermath() {
}

void CEDXAftermath::Create() {
	GPUCrashTracker.Create();

	const uint32_t AftermathFlags =
		GFSDK_Aftermath_FeatureFlags_EnableMarkers |
		GFSDK_Aftermath_FeatureFlags_EnableResourceTracking |
		GFSDK_Aftermath_FeatureFlags_CallStackCapturing |
		GFSDK_Aftermath_FeatureFlags_GenerateShaderDebugInfo;

	//TODO:Add Device...
	// AFTERMATH_CHECK_ERROR(
	// 	GFSDK_Aftermath_DX12_Initialize(
	// 		GFSDK_Aftermath_Version_API,
	// 		AftermathFlags,
	// 		Device.Get()
	// 	)
	// );
}
