#pragma once
#include "CEDXDescriptorHeap.h"
#include "CEDXDeviceElement.h"

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
		Core::Common::CERef<CEDXResource> Resource;
		CEDXOfflineDescriptorHeap* Heap = nullptr;
		uint32 OfflineHeapIndex = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE OfflineHandle;
	private:
	};

	class CEDXConstantBufferView : public CEDXView {

	};

	class CEDXBaseShaderResourceView : public CEShaderResourceView, public CEDXView {

	};

	class CEDXBaseUnorderedAccessView : public CEUnorderedAccessView, public CEDXView {

	};

	class CEDXBaseRenderTargetView : public CERenderTargetView, public CEDXView {

	};

	class CEDXBaseDepthStencilView : public CEDepthStencilView, public CEDXView {

	};

	template <typename TBaseView>
	class CEDXBaseView : public TBaseView {

	};

	using CEDXRenderTargetView = CEDXBaseView<CEDXBaseRenderTargetView>;
	using CEDXDepthStencilView = CEDXBaseView<CEDXBaseDepthStencilView>;
	using CEDXUnorderedAccessView = CEDXBaseView<CEDXBaseUnorderedAccessView>;
	using CEDXShaderResourceView = CEDXBaseView<CEDXBaseShaderResourceView>;
}
