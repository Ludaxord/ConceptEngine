#pragma once
#include <d3d12.h>
#include "../Libraries/d3dx12.h"
#include "CECubeMapFace.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	class CEShadowMap {
	public:
		CEShadowMap(ID3D12Device* device, UINT width, UINT height);
		CEShadowMap(const CEShadowMap& rhs) = delete;
		CEShadowMap& operator=(const CEShadowMap& rhs) = delete;
		~CEShadowMap() = default;

		UINT Width() const;
		UINT Height() const;
		ID3D12Resource* Resource();

		CD3DX12_GPU_DESCRIPTOR_HANDLE Srv() const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE Dsv() const;

		D3D12_VIEWPORT Viewport() const;
		D3D12_RECT ScissorRect() const;

		void BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		                      CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		                      CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv);

		void Resize(UINT newWidth, UINT newHeight);
	protected:
	private:
		void BuildDescriptors();
		void BuildResource();

		ID3D12Device* m_d3dDevice = nullptr;

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;

		UINT m_width = 0;
		UINT m_height = 0;
		DXGI_FORMAT Format = DXGI_FORMAT_R24G8_TYPELESS;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuDsv;

		Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
	};
}
