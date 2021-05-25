#pragma once
#include "CEDXDeviceElement.h"
#include "CEDXResource.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXBaseBuffer : public CEDXDeviceElement {
	public:
		CEDXBaseBuffer(CEDXDevice* device) : CEDXDeviceElement(device), Resource(nullptr) {

		}

		virtual void SetResource(CEDXResource* resource) {
			Resource = resource;
		}

		uint64 GetSizeInBytes() const {
			return Resource->GetDesc().Width;
		}

		CEDXResource* GetResource() {
			return Resource.Get();
		}

	protected:
		Core::Common::CERef<CEDXResource> Resource;
	};

	class CEDXBaseVertexBuffer : public CEVertexBuffer, public CEDXBaseBuffer {
	public:
		CEDXBaseVertexBuffer(CEDXDevice* device, uint32 numVertices, uint32 stride, uint32 flags):
			CEVertexBuffer(numVertices, stride, flags), CEDXBaseBuffer(device), View() {

		}

		virtual void SetResource(CEDXResource* resource) override {
			CEDXBaseBuffer::SetResource(resource);

			Memory::CEMemory::Memzero(&View);
			View.StrideInBytes = GetStride();
			View.SizeInBytes = GetNumVertices() * View.StrideInBytes;
			View.BufferLocation = Resource->GetGPUVirtualAddress();
		}

		const D3D12_VERTEX_BUFFER_VIEW& GetView() const {
			return View;
		}

	private:
		D3D12_VERTEX_BUFFER_VIEW View;
	};
}
