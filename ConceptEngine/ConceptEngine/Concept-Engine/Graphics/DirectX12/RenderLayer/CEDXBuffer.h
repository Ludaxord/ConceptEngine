#pragma once
#include "CEDXDescriptorHeap.h"
#include "CEDXDeviceElement.h"
#include "CEDXResource.h"
#include "CEDXViews.h"

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

	class CEDXBaseIndexBuffer : public CEIndexBuffer, public CEDXBaseBuffer {
	public:
		CEDXBaseIndexBuffer(CEDXDevice* device, CEIndexFormat indexFormat, uint32 numIndices, uint32 flags) :
			CEIndexBuffer(indexFormat, numIndices, flags), CEDXBaseBuffer(device), View() {

		}

		virtual void SetResource(CEDXResource* resource) override {
			CEDXBaseBuffer::SetResource(resource);
			Memory::CEMemory::Memzero(&View);

			CEIndexFormat indexFormat = GetFormat();
			if (indexFormat != CEIndexFormat::Unknown) {
				View.Format = indexFormat == CEIndexFormat::uint16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
				View.SizeInBytes = GetNumIndices() * GetStrideFromIndexFormat(indexFormat);
				View.BufferLocation = Resource->GetGPUVirtualAddress();
			}
		}

		const D3D12_INDEX_BUFFER_VIEW& GetView() const {
			return View;
		}

	private:
		D3D12_INDEX_BUFFER_VIEW View;
	};

	class CEDXBaseConstantBuffer : public CEConstantBuffer, public CEDXBaseBuffer {
	public:
		CEDXBaseConstantBuffer(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap, uint32 sizeInBytes, uint32 flags) :
			CEConstantBuffer(sizeInBytes, flags), CEDXBaseBuffer(device), View(device, heap) {

		}

		virtual void SetResource(CEDXResource* resource) override {
			CEDXBaseBuffer::SetResource(resource);
		}

		CEDXConstantBufferView& GetView() {
			return View;
		}

		const CEDXConstantBufferView& GetView() const {
			return View;
		}

	private:
		CEDXConstantBufferView View;
	};
}
