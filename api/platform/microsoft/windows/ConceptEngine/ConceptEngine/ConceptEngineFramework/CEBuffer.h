#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "CEResource.h"

namespace ConceptEngine::GraphicsEngine::Direct3D {
	namespace wrl = Microsoft::WRL;

	class CEBuffer : public CEResource {
	public:
	protected:
		CEBuffer(CEDevice& device, const D3D12_RESOURCE_DESC& resDesc);
		CEBuffer(CEDevice& device, wrl::ComPtr<ID3D12Resource> resource);
	};

};
