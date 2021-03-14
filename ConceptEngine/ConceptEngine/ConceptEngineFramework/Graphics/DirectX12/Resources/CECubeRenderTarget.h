#pragma once
#include <d3d12.h>
#include "../Libraries/d3dx12.h"


namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	enum class CubeMapFace : int {
		PositiveX = 0,
		NegativeX = 1,
		PositiveY = 2,
		NegativeY = 3,
		PositiveZ = 4,
		NegativeZ = 5
	};

	class CECubeRenderTarget {
	public:
		CECubeRenderTarget(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);
		CECubeRenderTarget(const CECubeRenderTarget& rhs) = delete;
		CECubeRenderTarget& operator=(const CECubeRenderTarget& rhs) = delete;
		~CECubeRenderTarget() = default;

		ID3D12Resource* Resource();
		CD3DX12_GPU_DESCRIPTOR_HANDLE Srv();
		CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv(int faceIndex);

		D3D12_VIEWPORT Viewport() const;
		D3D12_RECT ScissorRect() const;

		void BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		                      CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		                      CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv[6]);
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
		DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv[6];

		Microsoft::WRL::ComPtr<ID3D12Resource> m_cubeMap = nullptr;
	};
}
