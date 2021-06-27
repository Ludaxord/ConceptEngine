#pragma once
#include <dxgi1_4.h>

#include "CEDXCommandContext.h"
#include "CEDXDeviceElement.h"
#include "CEDXTexture.h"
#include "../../Main/RenderLayer/CEViewport.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXViewport : public CEViewport, public CEDXDeviceElement {
	public:
		CEDXViewport(CEDXDevice* device, CEDXCommandContext* commandContext, HWND hwnd, CEFormat format, uint32 width,
		             uint32 height);
		~CEDXViewport();

		bool Create();

		virtual bool Resize(uint32 width, uint32 height) override final;

		virtual bool Present(bool verticalSync) override final;

		virtual void SetName(const std::string& name) override final;

		virtual CERenderTargetView* GetRenderTargetView() const override final {
			return BackBufferViews[BackBufferIndex].Get();
		}

		virtual CETexture2D* GetBackBuffer() const override final {
			return BackBuffers[BackBufferIndex].Get();
		}

		virtual bool IsValid() const {
			return SwapChain != nullptr;
		}

		virtual void* GetNativeResource() const {
			return reinterpret_cast<void*>(SwapChain.Get());
		}

	private:
		bool RetrieveBackBuffers();

		Microsoft::WRL::ComPtr<IDXGISwapChain3> SwapChain;
		CEDXCommandContext* CommandContext;
		HWND Hwnd = 0;
		uint32 Flags = 0;
		uint32 NumBackBuffers = 0;
		uint32 BackBufferIndex = 0;

		CEArray<CERef<CEDXTexture2D>> BackBuffers;
		CEArray<CERef<CEDXRenderTargetView>> BackBufferViews;

	};
}
