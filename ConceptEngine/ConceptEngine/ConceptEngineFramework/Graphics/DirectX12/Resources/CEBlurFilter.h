#pragma once
#include <d3d12.h>
#include <dxgiformat.h>
#include <intsafe.h>
#include "../Libraries/d3dx12.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	class CEBlurFilter {
	public:
		/*
		 * Width and height should match dimensions of input texture to blur.
		 * Recreate when screen is resized
		 */
		CEBlurFilter(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);
		CEBlurFilter(const CEBlurFilter& rhs) = delete;
		CEBlurFilter& operator=(const CEBlurFilter& rhs) = delete;
		CEBlurFilter() = default;

		ID3D12Resource* Output();

		void BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
		                      CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
		                      UINT descriptorSize);

		void OnResize(UINT newWidth, UINT newHeight);

		/*
		 * Blurs the input texture blurCount times
		 */
		void Execute(ID3D12GraphicsCommandList* cmdList,
		             ID3D12RootSignature* rootSignature,
		             ID3D12PipelineState* horzBlurPso,
		             ID3D12PipelineState* vertBlurPso,
		             ID3D12Resource* input,
		             int blurCount);
	protected:
	private:
		std::vector<float> CalcGaussWeights(float sigma);
		void BuildDescriptors();
		void BuildResources();

		const int MaxBlurRadius = 5;

		ID3D12Device* m_d3dDevice = nullptr;

		UINT m_width = 0;
		UINT m_height = 0;
		DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuUav;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuUav;

		CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuUav;

		CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuUav;

		//Two for ping-ponging textures
		Microsoft::WRL::ComPtr<ID3D12Resource> m_blurMap0 = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_blurMap1 = nullptr;
	};
}
