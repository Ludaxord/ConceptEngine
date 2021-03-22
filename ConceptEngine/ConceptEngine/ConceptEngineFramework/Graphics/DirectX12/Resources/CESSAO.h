#ifndef SSAO_H
#define SSAO_H

#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>

#include "CEFrameResource.h"
#include "../Libraries/d3dx12.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	class CESSAO {
	public:
		CESSAO(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT width, UINT height);
		CESSAO(const CESSAO& rhs) = delete;
		CESSAO& operator=(const CESSAO& rhs) = delete;
		~CESSAO() = default;

		static const DXGI_FORMAT AmbientMapFormat = DXGI_FORMAT_R16_UNORM;
		static const DXGI_FORMAT NormalMapFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

		static const int MaxBlurRadius = 5;

		UINT SSAOMapWidth() const;
		UINT SSAOMapHeight() const;

		void GetOffsetVectors(DirectX::XMFLOAT4 offsets[14]);
		std::vector<float> CalcGaussWeights(float sigma);

		ID3D12Resource* NormalMap();
		ID3D12Resource* AmbientMap();

		CD3DX12_CPU_DESCRIPTOR_HANDLE NormalMapRtv() const;
		CD3DX12_GPU_DESCRIPTOR_HANDLE NormalMapSrv() const;
		CD3DX12_GPU_DESCRIPTOR_HANDLE AmbientMapSrv() const;

		void BuildDescriptors(
			ID3D12Resource* depthStencilBuffer,
			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
			CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv,
			UINT cbvSrvUavDescriptorSize,
			UINT rtvDescriptorSize
		);

		void RebuildDescriptors(ID3D12Resource* depthStencilBuffer);

		void SetPSOs(ID3D12PipelineState* ssaoPso, ID3D12PipelineState* ssaoBlurPso);

		/*
		 * Call when backBuffer is resized
		 */
		void Resize(UINT newWidth, UINT newHeight);

		/*
		 * Changes the render target to Ambient Render Target and Draw a fullscreen quad to kick off pixel shader to compute AmbientMap.
		 * We still keep the main depth buffer bind to pipeline, but depth buffer read/writes are disabled, so we do not need depth buffer computing Ambient map
		 */
		void ComputeSSAO(ID3D12GraphicsCommandList* cmdList, CEFrameResource* currFrame, int blurCount);
	protected:
	private:
		/*
		 * Blurs the ambient map to smotth out the nouse caused by only taking a few random samples per pixel.
		 * We use an edge preserving blur so that we do not blur across discontinuities -- we want edges to remain edges
		 */
		void BlurAmbientMap(ID3D12GraphicsCommandList* cmdList, CEFrameResource* currFrame, int blurCount);
		void BlurAmbientMap(ID3D12GraphicsCommandList* cmdList, bool horzBlur);

		void BuildResources();
		void BuildRandomVectorTexture(ID3D12GraphicsCommandList* cmdList);

		void BuildOffsetVectors();

		ID3D12Device* m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_SSAORootSignature;

		ID3D12PipelineState* m_SSAOPso = nullptr;
		ID3D12PipelineState* m_blurPso = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_randomVectorMap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_randomVectorMapUploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_normalMap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ambientMap0;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ambientMap1;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhNormalMapGpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuRtv;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhDepthMapCpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhDepthMapGpuSrv;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhRandomVectorMapCpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhRandomVectorMapGpuSrv;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap0CpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhAmbientMap0GpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap0CpuRtv;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap1CpuSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mhAmbientMap1GpuSrv;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap1CpuRtv;

		UINT m_renderTargetWidth;
		UINT m_renderTargetHeight;

		DirectX::XMFLOAT4 m_offsets[14];

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;
	};
}

#endif