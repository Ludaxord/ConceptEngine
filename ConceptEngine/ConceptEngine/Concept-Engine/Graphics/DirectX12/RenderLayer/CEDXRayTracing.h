#pragma once

#include "CEDXDeviceElement.h"
#include "../../Main/RenderLayer/CERayTracing.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {
	class CEDXRayTracingGeometry : public Main::RenderLayer::CERayTracingGeometry, public CEDXDeviceElement {
	public:
		CEDXRayTracingGeometry(CEDXDevice* device, uint32 flags);
		~CEDXRayTracingGeometry() = default;

		bool Build(class CEDXCommandContext& commandContext, bool update);

		virtual void SetName(const std::string& name) override {
			CEResource::SetName(name);
			ResultBuffer->SetName(name);
		}

		virtual bool IsValid() const override {
			return ResultBuffer != nullptr;
		}

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
			Assert(ResultBuffer != nullptr);
			return ResultBuffer->GetGPUVirtualAddress();
		}

		Core::Common::CERef<CEDXVertexBuffer> VertexBuffer;
		Core::Common::CERef<CEDXIndexBuffer> IndexBuffer;
		Core::Common::CERef<CEDXResource> ResultBuffer;
		Core::Common::CERef<CEDXResource> ScratchBuffer;
	};

	struct alignas(D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT) CEDXShaderBindingTableEntry {
		char ShaderIdentifier[D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES];
		D3D12_GPU_DESCRIPTOR_HANDLE RootDescriptorTables[4] = {0, 0, 0, 0};
	};

	class CEDXShaderBindingTableBuilder : CEDXDeviceElement {
	public:
		CEDXShaderBindingTableBuilder(CEDXDevice* device);
		~CEDXShaderBindingTableBuilder() = default;

		void PopulateEntry(CEDXRayTracingPipelineState* pipelineState, CEDXRootSignature* rootSignature,
		                   CEDXOnlineDescriptorHeap* resourceHeap, CEDXOnlineDescriptorHeap* samplerHeap,
		                   CEDXShaderBindingTableEntry& shaderBindingEntry,
		                   const CERayTracingShaderResources& resources);

		void CopyDescriptors();

		void Reset();
	private:
		uint32 CPUHandleSizes[1024];
		D3D12_CPU_DESCRIPTOR_HANDLE ResourceHandles[1024];
		D3D12_CPU_DESCRIPTOR_HANDLE SamplerHandles[1024];
		//Online
		D3D12_CPU_DESCRIPTOR_HANDLE GPUResourceHandles[1024];
		D3D12_CPU_DESCRIPTOR_HANDLE GPUSamplerHandles[1024];
		uint32 GPUResourceHandleSizes[1024];
		uint32 GPUSamplerHandleSizes[1024];
		uint32 CPUResourceIndex = 0;
		uint32 CPUSamplerIndex = 0;
		uint32 GPUResourceIndex = 0;
		uint32 GPUSamplerIndex = 0;
	};

	class CEDXRayTracingScene : public CERayTracingScene, public CEDXDeviceElement {
	public:
		CEDXRayTracingScene(CEDXDevice* device, uint32 flags);
		~CEDXRayTracingScene() = default;

		bool Build(class CEDXCommandContext& commandContext, const CERayTracingGeometryInstance* instances,
		           uint32 numInstances, bool update);

		bool BuildBindingTable(class CEDXCommandContext& commandContext, CEDXRayTracingPipelineState* pipelineState,
		                       CEDXOnlineDescriptorHeap* resourceHeap, CEDXOnlineDescriptorHeap* samplerHeap,
		                       const CERayTracingShaderResources* rayGenLocalResources,
		                       const CERayTracingShaderResources* missLocalResources,
		                       const CERayTracingShaderResources* hitGroupResources, uint32 numHitGroupResources);

		virtual void SetName(const std::string& name) override;

		virtual bool IsValid() const override {
			return ResultBuffer != nullptr;
		}

		virtual CEShaderResourceView* GetShaderResourceView() const override {
			return View.Get();
		}

		D3D12_GPU_VIRTUAL_ADDRESS_RANGE GetRayGenShaderRecord() const;
		D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE GetMissShaderTable() const;
		D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE GetHitGroupTable() const;

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
			Assert(ResultBuffer != nullptr);
			return ResultBuffer->GetGPUVirtualAddress();
		}

		CEDXResource* GetResultBuffer() const {
			return ResultBuffer.Get();
		}

		CEDXResource* GetInstanceBuffer() const {
			return InstanceBuffer.Get();
		}

		CEDXResource* GetBindingTable() const {
			return BindingTable.Get();
		}

	private:
		Core::Containers::CEArray<CERayTracingGeometryInstance> Instances;
		Core::Common::CERef<CEDXShaderResourceView> View;

		Core::Common::CERef<CEDXResource> ResultBuffer;
		Core::Common::CERef<CEDXResource> ScratchBuffer;
		Core::Common::CERef<CEDXResource> InstanceBuffer;
		Core::Common::CERef<CEDXResource> BindingTable;

		uint32 BindingTableStride = 0;
		uint32 NumHitGroups = 0;

		CEDXShaderBindingTableBuilder ShaderBindingTableBuilder;
		ID3D12DescriptorHeap* BindingTableHeaps[2] = {nullptr, nullptr};
	};

}
