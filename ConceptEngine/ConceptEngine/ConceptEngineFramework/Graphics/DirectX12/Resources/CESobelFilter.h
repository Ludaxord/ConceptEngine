#pragma once
#include <d3d12.h>
#include "../Libraries/d3dx12.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	class CESobelFilter {
	public:
		/*
		 * Width and height should match dimensions of input texture to apply filter
		 * Recreate when screen is resized
		 */
		CESobelFilter(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);
		CESobelFilter(const CESobelFilter& rhs) = delete;
		~CESobelFilter() = default;

		CD3DX12_GPU_DESCRIPTOR_HANDLE OutputSrv() const;

		UINT DescriptorCount() const;

		void BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
		                      CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
		                      UINT descriptorSize);

		void OnResize(UINT newWidth, UINT newHeight);

		void Execute(ID3D12GraphicsCommandList* cmdList,
		             ID3D12RootSignature* rootSignature,
		             ID3D12PipelineState* Pso,
		             CD3DX12_GPU_DESCRIPTOR_HANDLE input);
	protected:
	private:
		void BuildDescriptors();
		void BuildResource();

		ID3D12Device* m_d3dDevice = nullptr;

		UINT m_width = 0;
		UINT m_height = 0;
		DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuUav;

		CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuUav;

		Microsoft::WRL::ComPtr<ID3D12Resource> mOutput = nullptr;
	};
}
