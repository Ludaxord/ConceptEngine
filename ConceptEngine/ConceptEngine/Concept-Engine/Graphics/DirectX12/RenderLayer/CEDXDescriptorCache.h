#pragma once
#include "CEDXDeviceElement.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXDescriptorCache : public CEDXDeviceElement {
	public:
		CEDXDescriptorCache(CEDXDevice* device);
		~CEDXDescriptorCache();
		bool Create();	
	};

	class CEDXShaderConstantsCache {

	};
}
