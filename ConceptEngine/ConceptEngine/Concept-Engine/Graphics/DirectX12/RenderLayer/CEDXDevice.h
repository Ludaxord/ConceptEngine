#pragma once
#include "CEDXHelper.h"

#include "../Base/CEDirectXHandlers.h"

#include <DXProgrammableCapture.h>
#include <string>

#include "../../../Core/Common/CETypes.h"

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {

#define D3D12_PIPELINE_STATE_STREAM_ALIGNMENT (sizeof(void*));
#define D3D12_ENABLE_PIX_MARKERS 0;

	void DeviceRemovedHandler(class CEDXDevice* Device);

	enum class CreateOption {
		Lib,
		DLL
	};


	class CEDXDevice {
	public:
		CEDXDevice(bool enableDebugLayer, bool enableGPUValidation, bool EnableDRED);
		~CEDXDevice();

		bool Create(CreateOption create = CreateOption::DLL);

		int32 GetMultiSampleQuality(DXGI_FORMAT format, uint32 sampleCount);

		std::string GetAdapterName() const;

		HRESULT CreateRootSignature(UINT nodeMask,
		                            const void* blobWithRootSignature,
		                            SIZE_T blobLengthInBytes,
		                            REFIID riid, void** rootSignature) {
			return Device->CreateRootSignature(nodeMask, blobWithRootSignature, blobLengthInBytes, riid, rootSignature);
		}

		HRESULT CreateCommittedResource(const D3D12_HEAP_PROPERTIES* heapProperties,
		                                D3D12_HEAP_FLAGS heapFlags,
		                                const D3D12_RESOURCE_DESC* desc,
		                                D3D12_RESOURCE_STATES initialResourceState,
		                                const D3D12_CLEAR_VALUE* optimizedClearValue,
		                                REFIID riidResource,
		                                void** resource) {
			return Device->CreateCommittedResource(heapProperties,
			                                       heapFlags,
			                                       desc,
			                                       initialResourceState,
			                                       optimizedClearValue,
			                                       riidResource,
			                                       resource);
		}

		HRESULT CreatePipelineState(const D3D12_PIPELINE_STATE_STREAM_DESC* desc, REFIID riid, void** pipelineState) {
			return DXRDevice->CreatePipelineState(desc, riid, pipelineState);
		}

		void CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* desc,
		                              D3D12_CPU_DESCRIPTOR_HANDLE descDescriptor) {
			Device->CreateConstantBufferView(desc, descDescriptor);
		}

		void CreateRenderTargetView(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc,
		                            D3D12_CPU_DESCRIPTOR_HANDLE destinationDesc) {
			Device->CreateRenderTargetView(resource, desc, destinationDesc);
		}

		void CreateDepthStencilView(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc,
		                            D3D12_CPU_DESCRIPTOR_HANDLE destinationDesc) {
			Device->CreateDepthStencilView(resource, desc, destinationDesc);
		}

		void CreateShaderResourceView(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc,
		                              D3D12_CPU_DESCRIPTOR_HANDLE destinationDesc) {
			Device->CreateShaderResourceView(resource, desc, destinationDesc);
		}

		void CreateUnorderedAccessView(ID3D12Resource* resource, ID3D12Resource* counterResource,
		                               const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc,
		                               D3D12_CPU_DESCRIPTOR_HANDLE destinationDesc) {
			Device->CreateUnorderedAccessView(resource, counterResource, desc, destinationDesc);
		}

		void CreateSampler(const D3D12_SAMPLER_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE destinationDesc) {
			Device->CreateSampler(desc, destinationDesc);
		}

		void CopyDescriptors(uint32 numDestinationDescRanges,
		                     const D3D12_CPU_DESCRIPTOR_HANDLE* destinationDescRangeStart,
		                     const uint32* destinationDescRangeSizes,
		                     uint32 numSrcDescRanges,
		                     const D3D12_CPU_DESCRIPTOR_HANDLE* srcDescRangeStarts,
		                     const uint32* srcDescRangeSizes,
		                     D3D12_DESCRIPTOR_HEAP_TYPE descHeapType) {
			Device->CopyDescriptors(numDestinationDescRanges,
			                        destinationDescRangeStart,
			                        destinationDescRangeSizes,
			                        numSrcDescRanges,
			                        srcDescRangeStarts,
			                        srcDescRangeSizes,
			                        descHeapType);
		}

		void CopyDescriptorsSimple(uint32 numDescriptors,
		                           D3D12_CPU_DESCRIPTOR_HANDLE destinationDescRangeStart,
		                           D3D12_CPU_DESCRIPTOR_HANDLE srcDescRangeStart,
		                           D3D12_DESCRIPTOR_HEAP_TYPE descHeapsType) {
			Device->CopyDescriptorsSimple(numDescriptors, destinationDescRangeStart, srcDescRangeStart, descHeapsType);
		}

		void GetRayTracingAccelerationStructurePreBuildInfo(
			const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS* desc,
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO* info) {
			DXRDevice->GetRaytracingAccelerationStructurePrebuildInfo(desc, info);
		}

		uint32 GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) {
			return Device->GetDescriptorHandleIncrementSize(descriptorHeapType);
		}

		ID3D12Device* GetDevice() const {
			return Device.Get();
		}

		ID3D12Device5* GetDXRDevice() const {
			return DXRDevice.Get();
		}

		IDXGraphicsAnalysis* GetPIXCaptureInterface() const {
			return PIXCaptureInterface.Get();
		}

		IDXGIFactory2* GetFactory() const {
			return Factory.Get();
		}

		IDXGIAdapter1* GetAdapter() const {
			return Adapter.Get();
		}

		bool IsTearingSupported() const {
			return AllowTearing;
		}

		D3D12_RAYTRACING_TIER GetRayTracingTier() const {
			return RayTracingTier;
		}

		D3D12_SAMPLER_FEEDBACK_TIER GetSamplerFeedbackTier() const {
			return SamplerFeedBackTier;
		}

		D3D12_VARIABLE_SHADING_RATE_TIER GetVariableRateShadingTier() const {
			return VariableShadingRateTier;
		}

		D3D12_MESH_SHADER_TIER GetMeshShadowTier() const {
			return MeshShaderTier;
		}

		uint32 GetVariableRateShadingTileSize() const {
			return VariableShadingRateTileSize;
		}

	protected:
		bool CreateLib();
		bool CreateDLL();

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory2> Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> Adapter;
		Microsoft::WRL::ComPtr<ID3D12Device> Device;
		Microsoft::WRL::ComPtr<ID3D12Device5> DXRDevice;

		Microsoft::WRL::ComPtr<IDXGraphicsAnalysis> PIXCaptureInterface;

		D3D_FEATURE_LEVEL MinFeatureLevel = D3D_FEATURE_LEVEL_12_0;
		D3D_FEATURE_LEVEL ActiveFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		D3D12_RAYTRACING_TIER RayTracingTier = D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
		D3D12_SAMPLER_FEEDBACK_TIER SamplerFeedBackTier = D3D12_SAMPLER_FEEDBACK_TIER_NOT_SUPPORTED;
		D3D12_MESH_SHADER_TIER MeshShaderTier = D3D12_MESH_SHADER_TIER_NOT_SUPPORTED;
		D3D12_VARIABLE_SHADING_RATE_TIER VariableShadingRateTier = D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED;
		uint32 VariableShadingRateTileSize = 0;

		HMODULE DXGILib = 0;
		HMODULE D3D12Lib = 0;
		HMODULE PIXLib = 0;

		uint32 AdapterID = 0;

		bool AllowTearing = false;
		bool EnableDebugLayer = false;
		bool EnableGPUValidation = false;
		bool EnableDRED = false;

		CreateOption DXFunc;
	};
}
