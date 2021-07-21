#pragma once
#include <d3d12.h>
#include <GFSDK_Aftermath.h>

#include "CEDXDeviceElement.h"
#include "../../../Core/Debug/Nsight/CENsightAftermathGPUCrashTracker.h"

#ifndef USE_NSIGHT_AFTERMATH
#define USE_NSIGHT_AFTERMATH 1
#endif

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXAftermath : public CEDXDeviceElement {
	public:
		CEDXAftermath(CEDXDevice* Device);

		void Create(CEDXCommandContext& commandContext);

	private:
		GFSDK_Aftermath_ContextHandle AftermathCommandListContext;
		Core::Debug::Nsight::CENsightAftermathGPUCrashTracker GPUCrashTracker;
	};
}
