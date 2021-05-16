#pragma once
#include "CEDXDeviceElement.h"

#include "../../../Main/RenderLayer/CEGPUProfiler.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::RenderLayer {
	class CEDXGPUProfiler : public Main::RenderLayer::CEGPUProfiler, public CEDXDeviceElement {

	};
}
