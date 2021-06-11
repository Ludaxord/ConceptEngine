#pragma once
#include "CEDXDescriptorHeap.h"
#include "CEDXDeviceElement.h"
#include "CEDXResource.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXView : public CEDXDeviceElement {
	public:
		CEDXView(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap);
		virtual ~CEDXView();

		bool Create();

		D3D12_CPU_DESCRIPTOR_HANDLE GetOfflineHandle() const {
			return OfflineHandle;
		}

		const CEDXResource* GetResource() const {
			return Resource.Get();
		}

	protected:
		CERef<CEDXResource> Resource;
		CEDXOfflineDescriptorHeap* Heap = nullptr;
		uint32 OfflineHeapIndex = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE OfflineHandle;
	private:
	};

	class CEDXConstantBufferView : public CEDXView {
	public:
		CEDXConstantBufferView(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap);
		~CEDXConstantBufferView() = default;

		bool CreateView(CEDXResource* resource, const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);

		const D3D12_CONSTANT_BUFFER_VIEW_DESC& GetDesc() const {
			return Desc;
		}

	protected:
	private:
		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
	};

	class CEDXBaseShaderResourceView : public CEShaderResourceView, public CEDXView {
	public:
		CEDXBaseShaderResourceView(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap);
		~CEDXBaseShaderResourceView() = default;

		bool CreateView(CEDXResource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

		const D3D12_SHADER_RESOURCE_VIEW_DESC& GetDesc() const {
			return Desc;
		}

	protected:
	private:
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc;
	};

	class CEDXBaseUnorderedAccessView : public CEUnorderedAccessView, public CEDXView {
	public:
		CEDXBaseUnorderedAccessView(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap);
		~CEDXBaseUnorderedAccessView() = default;

		bool CreateView(CEDXResource* counterResource, CEDXResource* resource,
		                const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);

		const D3D12_UNORDERED_ACCESS_VIEW_DESC& GetDesc() const {
			return Desc;
		}

		const CEDXResource* GetCounterResource() const {
			return CounterResource.Get();
		}

	protected:
	private:
		CERef<CEDXResource> CounterResource;
		D3D12_UNORDERED_ACCESS_VIEW_DESC Desc;
	};

	class CEDXBaseRenderTargetView : public CERenderTargetView, public CEDXView {
	public:
		CEDXBaseRenderTargetView(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap);
		~CEDXBaseRenderTargetView() = default;

		bool CreateView(CEDXResource* resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc);

		const D3D12_RENDER_TARGET_VIEW_DESC& GetDesc() const {
			return Desc;
		}

	protected:
	private:
		D3D12_RENDER_TARGET_VIEW_DESC Desc;
	};

	class CEDXBaseDepthStencilView : public CEDepthStencilView, public CEDXView {
	public:
		CEDXBaseDepthStencilView(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap);
		~CEDXBaseDepthStencilView() = default;

		bool CreateView(CEDXResource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);

		const D3D12_DEPTH_STENCIL_VIEW_DESC& GetDesc() const {
			return Desc;
		}

	protected:
	private:
		D3D12_DEPTH_STENCIL_VIEW_DESC Desc;
	};

	template <typename TBaseView>
	class CEDXBaseView : public TBaseView {
	public:
		CEDXBaseView(CEDXDevice* device, CEDXOfflineDescriptorHeap* heap) : TBaseView(device, heap) {

		};

		~CEDXBaseView() = default;

		virtual bool IsValid() const override {
			return this->OfflineHandle != 0;
		}

	protected:
	private:
	};

	using CEDXRenderTargetView = CEDXBaseView<CEDXBaseRenderTargetView>;
	using CEDXDepthStencilView = CEDXBaseView<CEDXBaseDepthStencilView>;
	using CEDXUnorderedAccessView = CEDXBaseView<CEDXBaseUnorderedAccessView>;
	using CEDXShaderResourceView = CEDXBaseView<CEDXBaseShaderResourceView>;
}
