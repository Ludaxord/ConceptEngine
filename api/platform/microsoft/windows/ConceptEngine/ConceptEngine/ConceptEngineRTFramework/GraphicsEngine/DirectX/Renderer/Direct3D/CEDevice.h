#pragma once
#include <d3d12.h>
#include <memory>
#include <string>
#include <vector>
#include <wrl.h>

namespace Concept::GraphicsEngine::DirectX::Renderer::Direct3D {
	namespace wrl = Microsoft::WRL;

	class CEGUI;
	class CEDescriptorAllocator;
	class CECommandQueue;
	class CEUnorderedAccessView;
	class CEResource;
	class CECommandList;
	class CEShaderResourceView;
	class CEConstantBufferView;
	class CEPipelineStateObject;
	class CERootSignature;
	class CEVertexBuffer;
	class CEIndexBuffer;
	class CETexture;
	class CEStructuredBuffer;
	class CEByteAddressBuffer;
	class CEConstantBuffer;
	class CERenderTarget;
	class CESwapChain;
	class CEAdapter;
	class CEScene;

	class CEDevice {
	public:
		/**
		 * Debug Layers are abe to catch possible error while creating or updating scene and display it in console.
		 */
		static void EnableDebugLayer();
		static void ReportLiveObjects();
	protected:
	private:
	};
}
