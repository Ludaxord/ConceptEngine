#pragma once
#include "CEDXDeviceElement.h"
#include "CEDXResource.h"

#include "../../Main/RenderLayer/CEGPUProfiler.h"
#

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXGPUProfiler : public Main::RenderLayer::CEGPUProfiler, public CEDXDeviceElement {
	public:
		CEDXGPUProfiler(CEDXDevice* device);
		~CEDXGPUProfiler() = default;

		virtual void GetTimeQuery(TimeQuery& query, uint32 index) const override final;

		virtual uint64 GetFrequency() const override final {
			return (uint64)Frequency;
		}

		void BeginQuery(ID3D12GraphicsCommandList* commandList, uint32 index);
		void EndQuery(ID3D12GraphicsCommandList* commandList, uint32 index);

		void ResolveQueries(class CEDXCommandContext& commandContext);

		ID3D12QueryHeap* GetQueryHeap() const {
			return QueryHeap.Get();
		}

		static CEDXGPUProfiler* Create(CEDXDevice* device);

	private:
		bool AllocateReadResources();

		Microsoft::WRL::ComPtr<ID3D12QueryHeap> QueryHeap;
		CERef<CEDXResource> WriteResource;

		CEArray<CERef<CEDXResource>> ReadResources;
		CEArray<TimeQuery> TimeQueries;

		UINT64 Frequency;
	};
}
