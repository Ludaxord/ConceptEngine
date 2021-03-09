#pragma once

#ifndef GPUWAVES_H
#define GPUWAVES_H

#include "CETypes.h"
#include "../../../Game/CETimer.h"
#include "../Libraries/d3dx12.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	class CEGpuWaves final : CEObject {
	public:
		//Note that m,n should be divisible by 16 so there is no remainder when we divide into thread groups
		CEGpuWaves(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, int m, int n, float dx, float dt,
		           float speed, float damping);
		CEGpuWaves(const CEGpuWaves& rhs) = delete;
		CEGpuWaves& operator=(const CEGpuWaves& rhs) = delete;
		~CEGpuWaves() = default;

		CD3DX12_GPU_DESCRIPTOR_HANDLE DisplacementMap() const;

		UINT DescriptorCount() const;

		void BuildResources(ID3D12GraphicsCommandList* cmdList);

		void BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
		                      CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
		                      UINT descriptorSize);

		void Update(const CETimer& gt,
		            ID3D12GraphicsCommandList* cmdList,
		            ID3D12RootSignature* rootSignature,
		            ID3D12PipelineState* Pso,
		            UINT i,
		            UINT j,
		            float magnitude);

		void Disturb(ID3D12GraphicsCommandList* cmdList,
		             ID3D12RootSignature* rootSignature,
		             ID3D12PipelineState* pso,
		             UINT i,
		             UINT j,
		             float magnitude);

		int RowCount() const override;
		int ColumnCount() const override;
		int VertexCount() const override;
		int TriangleCount() const override;
		float Width() const override;
		float Depth() const override;
		float SpatialStep()const;

		void Update(float dt) override;
		void Disturb(int i, int j, float magnitude) override;

	protected:
	private:
		UINT m_numRows;
		UINT m_numCols;

		UINT m_vertexCount;
		UINT m_triangleCount;

		//Simulation constants we can precompute
		float mK[3];

		float m_timeStep;
		float m_spatialStep;

		ID3D12Device* m_d3dDevice = nullptr;

		CD3DX12_GPU_DESCRIPTOR_HANDLE mPrevSolSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mCurrSolSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mNextSolSrv;

		CD3DX12_GPU_DESCRIPTOR_HANDLE mPrevSolUav;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mCurrSolUav;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mNextSolUav;

		//Two for ping-ponging textures
		Microsoft::WRL::ComPtr<ID3D12Resource> mPrevSol = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> mCurrSol = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> mNextSol = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> mPrevUploadBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> mCurrUploadBuffer = nullptr;
	};
}

#endif
