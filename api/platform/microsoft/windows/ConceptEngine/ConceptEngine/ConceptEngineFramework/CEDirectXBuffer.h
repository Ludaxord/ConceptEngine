#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "CEDirectXResource.h"

namespace ConceptEngine::GraphicsEngine::DirectX {
	namespace wrl = Microsoft::WRL;

	class CEDirectXBuffer : public CEDirectXResource {
	public:
	protected:
		CEDirectXBuffer(CEDirectXDevice& device, const D3D12_RESOURCE_DESC& resDesc);
		CEDirectXBuffer(CEDirectXDevice& device, wrl::ComPtr<ID3D12Resource> resource);
	};

};
