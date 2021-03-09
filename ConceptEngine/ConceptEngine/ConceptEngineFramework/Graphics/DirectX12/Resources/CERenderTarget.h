#pragma once
#include <d3d12.h>
#include "../Libraries/d3dx12.h"


namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	class CERenderTarget {
	public:
		///<summary>
		/// The width and height should match the dimensions of the input texture to blur.
		/// Recreate when the screen is resized. 
		///</summary>
		CERenderTarget(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);
		CERenderTarget(const CERenderTarget& rhs) = delete;
		CERenderTarget& operator=(const CERenderTarget& rhs) = delete;
		~CERenderTarget() = default;

		ID3D12Resource* Resource();
		CD3DX12_GPU_DESCRIPTOR_HANDLE Srv();
		CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv();

		void BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		                      CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		                      CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv);

		void OnResize(UINT newWidth, UINT newHeight);
	protected:
	private:
		void BuildDescriptors();
		void BuildResource();

		ID3D12Device* m_d3dDevice = nullptr;
		UINT m_width = 0;
		UINT m_height = 0;
		DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv;

		Microsoft::WRL::ComPtr<ID3D12Resource> mOffscreenTex = nullptr;
	};
}
