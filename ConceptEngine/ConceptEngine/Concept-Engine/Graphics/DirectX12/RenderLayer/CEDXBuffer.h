#pragma once
#include "CEDXDescriptorHeap.h"
#include "CEDXDeviceElement.h"
#include "CEDXResource.h"
#include "CEDXViews.h"
#include "../../../Core/Containers/CEContainerUtilities.h"

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
		CERef<CEDXResource> Resource;
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

			D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc;
			Memory::CEMemory::Memzero(&viewDesc);

			viewDesc.BufferLocation = Resource->GetGPUVirtualAddress();
			viewDesc.SizeInBytes = (uint32)GetSizeInBytes();

			if (View.GetOfflineHandle() == 0) {
				if (!View.Create()) {
					return;
				}
			}

			View.CreateView(Resource.Get(), viewDesc);
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

	class CEDXBaseStructuredBuffer : public CEStructuredBuffer, public CEDXBaseBuffer {
	public:
		CEDXBaseStructuredBuffer(CEDXDevice* device, uint32 sizeInBytes, uint32 stride, uint32 flags) :
			CEStructuredBuffer(sizeInBytes, stride, flags), CEDXBaseBuffer(device) {

		}
	};

	template <typename TBaseBuffer>
	class TCEDXBaseBuffer : public TBaseBuffer {
	public:
		template <typename... TBufferArgs>
		TCEDXBaseBuffer(CEDXDevice* device, TBufferArgs&&... BufferArgs) : TBaseBuffer(
			device, Forward<TBufferArgs>(BufferArgs)...) {
		}

		virtual void* Map(uint32 offset, uint32 size) override {
			if (offset != 0 || size != 0) {
				D3D12_RANGE mapRange = {offset, size};
				return CEDXBaseBuffer::Resource->Map(0, &mapRange);
			}
			else {
				return CEDXBaseBuffer::Resource->Map(0, nullptr);
			}
		}

		virtual void Unmap(uint32 offset, uint32 size) override {
			if (offset != 0 || size != 0) {
				D3D12_RANGE range = {offset, size};
				CEDXBaseBuffer::Resource->Unmap(0, &range);
			} else {
				CEDXBaseBuffer::Resource->Unmap(0, nullptr);
			}
		}

		virtual void SetName(const std::string& name) override final {
			CEResource::SetName(name);
			CEDXBaseBuffer::Resource->SetName(name);
		}

		virtual void* GetNativeResource() const override final {
			return reinterpret_cast<void*>(CEDXBaseBuffer::Resource->GetResource());
		}

		virtual bool IsValid() const override {
			return CEDXBaseBuffer::Resource->GetResource() != nullptr;
		}
	};

	class CEDXVertexBuffer : public TCEDXBaseBuffer<CEDXBaseVertexBuffer> {
	public:
		CEDXVertexBuffer(CEDXDevice* device, uint32 numVertices, uint32 stride, uint32 flags) : TCEDXBaseBuffer<
			CEDXBaseVertexBuffer>(device, numVertices, stride, flags) {

		}
	};

	class CEDXIndexBuffer : public TCEDXBaseBuffer<CEDXBaseIndexBuffer> {
	public:
		CEDXIndexBuffer(CEDXDevice* device, CEIndexFormat indexFormat, uint32 numIndices, uint32 flags) :
			TCEDXBaseBuffer<CEDXBaseIndexBuffer>(device, indexFormat, numIndices, flags) {

		}
	};

	class CEDXConstantBuffer : public TCEDXBaseBuffer<CEDXBaseConstantBuffer> {
	public:
		CEDXConstantBuffer(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap, uint32 sizeInBytes, uint32 flags) :
			TCEDXBaseBuffer<CEDXBaseConstantBuffer>(device, heap, sizeInBytes, flags) {
		}
	};

	class CEDXStructuredBuffer : public TCEDXBaseBuffer<CEDXBaseStructuredBuffer> {
	public:
		CEDXStructuredBuffer(CEDXDevice* device, uint32 numElements, uint32 stride, uint32 flags) : TCEDXBaseBuffer<
			CEDXBaseStructuredBuffer>(device, numElements, stride, flags) {

		}
	};

	inline CEDXBaseBuffer* CEDXBufferCast(CEBuffer* buffer) {
		if (buffer->AsVertexBuffer()) {
			return static_cast<CEDXVertexBuffer*>(buffer);
		}
		else if (buffer->AsIndexBuffer()) {
			return static_cast<CEDXIndexBuffer*>(buffer);
		}
		else if (buffer->AsConstantBuffer()) {
			return static_cast<CEDXConstantBuffer*>(buffer);
		}
		else if (buffer->AsStructuredBuffer()) {
			return static_cast<CEDXStructuredBuffer*>(buffer);
		}
		else {
			return nullptr;
		}
	}
}
