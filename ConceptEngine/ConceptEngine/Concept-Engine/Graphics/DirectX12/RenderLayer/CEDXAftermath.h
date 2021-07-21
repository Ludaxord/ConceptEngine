#pragma once

#include <d3d12.h>
#include "../../../Core/Debug/Nsight/CENsightAftermathGPUCrashTracker.h"

#include <GFSDK_Aftermath.h>
#include <GFSDK_Aftermath_Defines.h>
#include <GFSDK_Aftermath_GpuCrashDump.h>
#include <GFSDK_Aftermath_GpuCrashDumpDecoding.h>

#ifndef USE_NSIGHT_AFTERMATH
#define USE_NSIGHT_AFTERMATH 1
#endif

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXDevice;

	class CEDXAftermath {
	public:
		CEDXAftermath();

		bool CreateAMCommandList(ID3D12CommandList* D3DCommandList);

		bool CreateCrashTracker();

		bool Create(CEDXDevice* Device) const;
	private:
		GFSDK_Aftermath_ContextHandle AftermathCommandListContext;
		Core::Debug::Nsight::CENsightAftermathGPUCrashTracker GPUCrashTracker;
	};
}

extern ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXAftermath Aftermath;