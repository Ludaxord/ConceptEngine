#pragma once
#include "CEDXBuffer.h"
#include "CEDXCommandList.h"
#include "CEDXDeviceElement.h"
#include "CEDXSamplerState.h"
#include "CEDXShader.h"

#include "../../../Math/CEMath.h"

#define NUM_VISIBILITIES (ShaderVisibility_Count)
#define NUM_DESCRIPTORS (D3D12_MAX_ONLINE_DESCRIPTOR_COUNT / 4)

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	template <typename TCEDXDescriptorView>
	struct TCEDXDescriptorViewCache {
		TCEDXDescriptorViewCache(): DescriptorViews(), Descriptors(), CopyDescriptors(), Dirty(),
		                            DescriptorRangeLengths(), TotalNumDescriptors(0) {
			Reset();
		}

		void Set(TCEDXDescriptorView* descriptorView, CEShaderVisibility visibility, uint32 shaderRegister) {
			Assert(descriptorView != nullptr);
			TCEDXDescriptorView* currentDescriptorView = DescriptorViews[visibility][shaderRegister];
			if (descriptorView != currentDescriptorView) {
				DescriptorViews[visibility][shaderRegister] = descriptorView;
				Dirty[visibility] = true;

				uint32& rangeLength = DescriptorRangeLengths[visibility];
				rangeLength = ConceptEngine::Math::CEMath::Max<uint32>(rangeLength, shaderRegister + 1);
			}
		}

		void Reset() {
			ConceptEngine::Memory::CEMemory::Memzero(DescriptorViews, sizeof(DescriptorViews));
			ConceptEngine::Memory::CEMemory::Memzero(Descriptors, sizeof(Descriptors));
			ConceptEngine::Memory::CEMemory::Memzero(CopyDescriptors, sizeof(CopyDescriptors));
			ConceptEngine::Memory::CEMemory::Memzero(DescriptorRangeLengths, sizeof(DescriptorRangeLengths));

			for (uint32 i = 0; i < ShaderVisibility_Count; i++) {
				Dirty[i] = true;
			}
		}

		uint32 CountNeededDescriptors() const {
			uint32 numDescriptors = 0;
			for (uint32 i = 0; i < ShaderVisibility_Count; i++) {
				if (Dirty[i]) {
					numDescriptors += DescriptorRangeLengths[i];
				}
			}

			return numDescriptors;
		}

		void PrepareForCopy(TCEDXDescriptorView* defaultView) {
			TotalNumDescriptors = 0;
			for (uint32 i = 0; i < ShaderVisibility_Count; i++) {
				if (Dirty[i]) {
					uint32 numDescriptors = DescriptorRangeLengths[i];
					uint32 offset = TotalNumDescriptors;

					TotalNumDescriptors += numDescriptors;
					Assert(TotalNumDescriptors <= NUM_DESCRIPTORS);

					for (uint32 descIndex = 0; descIndex < numDescriptors; descIndex++) {
						TCEDXDescriptorView* view = DescriptorViews[i][descIndex];
						if (!view) {
							DescriptorViews[i][descIndex] = view = defaultView;
						}
						CopyDescriptors[offset + descIndex] = view->GetOfflineHandle();
					}
				}
			}

		}

		void SetGPUHandles(D3D12_GPU_DESCRIPTOR_HANDLE startHandle, uint64 descriptorSize) {
			for (uint32 i = 0; i < ShaderVisibility_Count; i ++) {
				if (Dirty[i]) {
					Descriptors[i] = startHandle;
					startHandle.ptr += (uint64)DescriptorRangeLengths[i] * descriptorSize;

					Dirty[i] = false;
				}
			}
		}

		void InvalidateAll() {
			for (uint32 i = 0; i < NUM_VISIBILITIES; i++) {
				Dirty[i] = true;
			}
		}

		TCEDXDescriptorView* DescriptorViews[NUM_VISIBILITIES][NUM_DESCRIPTORS];
		D3D12_GPU_DESCRIPTOR_HANDLE Descriptors[NUM_VISIBILITIES];
		D3D12_CPU_DESCRIPTOR_HANDLE CopyDescriptors[NUM_DESCRIPTORS];
		bool Dirty[NUM_VISIBILITIES];
		uint32 DescriptorRangeLengths[NUM_VISIBILITIES];
		uint32 TotalNumDescriptors;
	};

	using CEDXConstantBufferViewCache = TCEDXDescriptorViewCache<CEDXConstantBufferView>;
	using CEDXShaderResourceViewCache = TCEDXDescriptorViewCache<CEDXShaderResourceView>;
	using CEDXUnorderedAccessViewCache = TCEDXDescriptorViewCache<CEDXUnorderedAccessView>;
	using CEDXSamplerStateCache = TCEDXDescriptorViewCache<CEDXSamplerState>;

	class CEDXVertexBufferCache {
	public:
		CEDXVertexBufferCache(): VertexBuffers(),
		                         VertexBufferViews(),
		                         NumVertexBuffers(0),
		                         VertexBuffersDirty(false),
		                         IndexBuffer(nullptr),
		                         IndexBufferView(),
		                         IndexBufferDirty(false) {
			Reset();
		}

		void SetVertexBuffer(CEDXVertexBuffer* vertexBuffer, uint32 slot) {
			Assert(slot < D3D12_MAX_VERTEX_BUFFER_SLOTS);

			if (VertexBuffers[slot] != vertexBuffer) {
				VertexBuffers[slot] = vertexBuffer;
				NumVertexBuffers = ConceptEngine::Math::CEMath::Max(NumVertexBuffers, slot + 1);

				VertexBuffersDirty = true;
			}
		}

		void SetIndexBuffer(CEDXIndexBuffer* indexBuffer) {
			if (IndexBuffer != indexBuffer) {
				IndexBuffer = indexBuffer;
				IndexBufferDirty = true;
			}
		}

		void CommitState(CEDXCommandListHandle& commandList) {
			ID3D12GraphicsCommandList* dxCommandList = commandList.GetGraphicsCommandList();
			if (VertexBuffersDirty) {
				for (uint32 i = 0; i < NumVertexBuffers; i++) {
					CEDXVertexBuffer* vertexBuffer = VertexBuffers[i];
					if (!vertexBuffer) {
						VertexBufferViews[i].BufferLocation = 0;
						VertexBufferViews[i].SizeInBytes = 0;
						VertexBufferViews[i].StrideInBytes = 0;
					}
					else {
						VertexBufferViews[i] = vertexBuffer->GetView();
					}
				}

				dxCommandList->IASetVertexBuffers(0, NumVertexBuffers, VertexBufferViews);
				VertexBuffersDirty = false;
			}

			if (IndexBufferDirty) {
				if (!IndexBuffer) {
					IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
					IndexBufferView.BufferLocation = 0;
					IndexBufferView.SizeInBytes = 0;
				}
				else {
					IndexBufferView = IndexBuffer->GetView();
				}

				dxCommandList->IASetIndexBuffer(&IndexBufferView);
				IndexBufferDirty = false;
			}
		}

		void Reset() {
			Memory::CEMemory::Memzero(VertexBuffers, sizeof(VertexBuffers));
			NumVertexBuffers = 0;
			VertexBuffersDirty = true;
			IndexBuffer = nullptr;
			IndexBufferDirty = true;
		}

	private:
		CEDXVertexBuffer* VertexBuffers[D3D12_MAX_VERTEX_BUFFER_SLOTS];
		D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[D3D12_MAX_VERTEX_BUFFER_SLOTS];
		uint32 NumVertexBuffers;
		bool VertexBuffersDirty;

		CEDXIndexBuffer* IndexBuffer;
		D3D12_INDEX_BUFFER_VIEW IndexBufferView;
		bool IndexBufferDirty;
	};

	class CEDXRenderTargetCache {
	public:
		CEDXRenderTargetCache(): RenderTargetViewHandles(), NumRenderTargets(0), DepthStencilViewHandle({0}),
		                         DSVPtr(nullptr), Dirty(false) {
			Reset();
		}

		void SetRenderTargetView(CEDXRenderTargetView* renderTargetView, uint32 slot) {
			Assert(slot < D3D12_MAX_RENDER_TARGET_COUNT);
			if (renderTargetView) {
				RenderTargetViewHandles[slot] = renderTargetView->GetOfflineHandle();
			}
			else {
				RenderTargetViewHandles[slot] = {0};
			}

			NumRenderTargets = Math::CEMath::Max(NumRenderTargets, slot + 1);
			Dirty = true;
		}

		void SetDepthStencilView(CEDXDepthStencilView* depthStencilView) {
			if (depthStencilView) {
				DepthStencilViewHandle = depthStencilView->GetOfflineHandle();
				DSVPtr = &DepthStencilViewHandle;
			}
			else {
				DepthStencilViewHandle = {0};
				DSVPtr = nullptr;
			}

			Dirty = true;
		}

		void Reset() {
			Memory::CEMemory::Memzero(RenderTargetViewHandles, sizeof(RenderTargetViewHandles));
			DepthStencilViewHandle = {0};
			NumRenderTargets = 0;
		}

		void CommitState(CEDXCommandListHandle& commandList) {
			if (Dirty) {
				commandList.SetRenderTargets(RenderTargetViewHandles, NumRenderTargets, FALSE, DSVPtr);
				Dirty = false;
			}
		}

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandles[D3D12_MAX_RENDER_TARGET_COUNT];
		uint32 NumRenderTargets;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilViewHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE* DSVPtr;
		bool Dirty;
	};

	class CEDXDescriptorCache : public CEDXDeviceElement {
	public:
		CEDXDescriptorCache(CEDXDevice* device);
		~CEDXDescriptorCache();
		bool Create();

		void CommitGraphicsDescriptors(CEDXCommandListHandle& commandList, class CEDXCommandBatch* commandBatch,
		                               CEDXRootSignature* rootSignature);
		void CommitComputeDescriptors(CEDXCommandListHandle& commandList, class CEDXCommandBatch* commandBatch,
		                              CEDXRootSignature* rootSignature);

		void Reset();

		void SetVertexBuffer(CEDXVertexBuffer* vertexBuffer, uint32 slot) {
			VertexBufferCache.SetVertexBuffer(vertexBuffer, slot);
		}

		void SetIndexBuffer(CEDXIndexBuffer* indexBuffer) {
			VertexBufferCache.SetIndexBuffer(indexBuffer);
		}

		void SetRenderTargetView(CEDXRenderTargetView* renderTargetView, uint32 slot) {
			RenderTargetCache.SetRenderTargetView(renderTargetView, slot);
		}

		void SetDepthStencilView(CEDXDepthStencilView* depthStencilView) {
			RenderTargetCache.SetDepthStencilView(depthStencilView);
		}

		void SetConstantBufferView(CEDXConstantBufferView* descriptor, CEShaderVisibility visibility,
		                           uint32 shaderRegister) {
			if (!descriptor) {
				descriptor = NullCBV;
			}

			ConstantBufferViewCache.Set(descriptor, visibility, shaderRegister);
		}

		void SetShaderResourceView(CEDXShaderResourceView* descriptor, CEShaderVisibility visibility,
		                           uint32 shaderRegister) {
			if (!descriptor) {
				descriptor = NullSRV;
			}

			ShaderResourceViewCache.Set(descriptor, visibility, shaderRegister);
		}

		void SetUnorderedAccessView(CEDXUnorderedAccessView* descriptor, CEShaderVisibility visibility,
		                            uint32 shaderRegister) {
			if (!descriptor) {
				descriptor = NullUAV;
			}

			UnorderedAccessViewCache.Set(descriptor, visibility, shaderRegister);
		}

		void SetSamplerState(CEDXSamplerState* sampler, CEShaderVisibility visibility, uint32 shaderRegister) {
			if (!sampler) {
				sampler = NullSampler;
			}

			SamplerStateCache.Set(sampler, visibility, shaderRegister);
		}

	private:
		void CopyDescriptorsAndSetHeaps(ID3D12GraphicsCommandList* commandList, CEDXOnlineDescriptorHeap* resourceHeap,
		                                CEDXOnlineDescriptorHeap* samplerHeap);

		CEDXConstantBufferView* NullCBV = nullptr;
		CEDXShaderResourceView* NullSRV = nullptr;
		CEDXUnorderedAccessView* NullUAV = nullptr;
		CEDXSamplerState* NullSampler = nullptr;

		CEDXVertexBufferCache VertexBufferCache;
		CEDXRenderTargetCache RenderTargetCache;
		CEDXShaderResourceViewCache ShaderResourceViewCache;
		CEDXUnorderedAccessViewCache UnorderedAccessViewCache;
		CEDXConstantBufferViewCache ConstantBufferViewCache;
		CEDXSamplerStateCache SamplerStateCache;

		ID3D12DescriptorHeap* PreviousDescriptorHeaps[2] = {nullptr, nullptr};

		UINT RangeSizes[NUM_DESCRIPTORS];
	};

	class CEDXShaderConstantsCache {
	public:
		CEDXShaderConstantsCache() {
			Reset();
		}

		void Set32BitShaderConstants(uint32* constants, uint32 numConstants) {
			Assert(numConstants <= D3D12_MAX_32BIT_SHADER_CONSTANTS_COUNT);
			Memory::CEMemory::Memcpy(Constants, constants, sizeof(uint32) * numConstants);
			NumConstants = numConstants;
		}

		void CommitGraphics(CEDXCommandListHandle& commandList, CEDXRootSignature* rootSignature) {
			ID3D12GraphicsCommandList* dxCommandList = commandList.GetGraphicsCommandList();
			int32 rootIndex = rootSignature->Get32BitContantsIndex();
			if (rootIndex >= 0) {
				dxCommandList->SetGraphicsRoot32BitConstants(rootIndex, NumConstants, Constants, 0);
			}
		}

		void CommitCompute(CEDXCommandListHandle& commandList, CEDXRootSignature* rootSignature) {
			ID3D12GraphicsCommandList* dxCommandList = commandList.GetGraphicsCommandList();
			int32 rootIndex = rootSignature->Get32BitContantsIndex();
			if (rootIndex >= 0) {
				dxCommandList->SetComputeRoot32BitConstants(rootIndex, NumConstants, Constants, 0);
			}
		}

		void Reset() {
			Memory::CEMemory::Memzero(Constants, sizeof(Constants));
			NumConstants = 0;
		}

	private:
		uint32 Constants[D3D12_MAX_32BIT_SHADER_CONSTANTS_COUNT];
		uint32 NumConstants;
	};
}
