#include "CEDXManager.h"

#include "../RenderLayer/CEDXTexture.h"

#include "../../../Core/Application/CECore.h"

using namespace ConceptEngine::Graphics::DirectX12::Managers;
using namespace ConceptEngine::Graphics;

CEDXManager::CEDXManager(): CEGraphicsManager(), Device(nullptr), DirectCommandContext(nullptr) {
}

CEDXManager::~CEDXManager() {
	DirectCommandContext.Reset();

	//TODO: Safe delete all objects in private section
}

bool CEDXManager::Create() {
	bool gpuBasedValidationOn =
#if ENABLE_API_GPU_DEBUGGING
		Core::Application::CECore::EnableDebug;
#else
		false;
#endif

	bool DREDOn =
#if ENABLE_API_GPU_BREADCRUMBS
        Core::Application::CECore::EnableDebug;
#else
		false;
#endif

	Device = new RenderLayer::CEDXDevice(Core::Application::CECore::EnableDebug, gpuBasedValidationOn, DREDOn);
	if (!Device->Create()) {
		return false;
	}

	RootSignatureCache = new RenderLayer::CEDXRootSignatureCache(Device);
	if (!RootSignatureCache->Create()) {
		return false;
	}

	ResourceOfflineDescriptorHeap = new RenderLayer::CEDXOfflineDescriptorHeap(
		Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (!ResourceOfflineDescriptorHeap->Create()) {
		return false;
	}

	RenderTargetOfflineDescriptorHeap = new RenderLayer::CEDXOfflineDescriptorHeap(
		Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	if (!RenderTargetOfflineDescriptorHeap->Create()) {
		return false;
	}

	DepthStencilOfflineDescriptorHeap = new RenderLayer::CEDXOfflineDescriptorHeap(
		Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	if (!DepthStencilOfflineDescriptorHeap->Create()) {
		return false;
	}

	SamplerOfflineDescriptorHeap = new RenderLayer::CEDXOfflineDescriptorHeap(
		Device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	if (!SamplerOfflineDescriptorHeap->Create()) {
		return false;
	}

	DirectCommandContext = new RenderLayer::CEDXCommandContext(Device);
	if (!DirectCommandContext->Create()) {
		return false;
	}


	return true;
}

void CEDXManager::Destroy() {
}

Main::RenderLayer::CETexture2D* CEDXManager::CreateTexture2D(RenderLayer::CEFormat format, uint32 width, uint32 height, uint32 numMips,
                                                             uint32 numSamples, uint32 flags, RenderLayer::CEResourceState initialState,
                                                             const RenderLayer::CEResourceData* initialData,
                                                             const RenderLayer::CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTexture2D>(format, width, height, 1, numMips, numSamples, flags, initialState,
	                                                 initialData, optimizedClearValue);
}

Main::RenderLayer::CETexture2DArray* CEDXManager::CreateTexture2DArray(RenderLayer::CEFormat format, uint32 width, uint32 height, uint32 numMips,
                                                                       uint32 numSamples, uint32 numArraySlices, uint32 flags,
                                                                       RenderLayer::CEResourceState initialState,
                                                                       const RenderLayer::CEResourceData* initialData,
                                                                       const RenderLayer::CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTexture2DArray>(format, width, height, numArraySlices, numMips, numSamples,
	                                                      flags, initialState, initialData, optimizedClearValue);
}

Main::RenderLayer::CETextureCube* CEDXManager::CreateTextureCube(RenderLayer::CEFormat format,
                                                                 uint32 size,
                                                                 uint32 numMips,
                                                                 uint32 flags,
                                                                 RenderLayer::CEResourceState initialState,
                                                                 const RenderLayer::CEResourceData* initialData,
                                                                 const RenderLayer::CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTextureCube>(format, size, size, TEXTURE_CUBE_FACE_COUNT, numMips, 1,
	                                                   flags, initialState, initialData, optimizedClearValue);
}

Main::RenderLayer::CETextureCubeArray* CEDXManager::CreateTextureCubeArray(RenderLayer::CEFormat format,
                                                                           uint32 size,
                                                                           uint32 numMips,
                                                                           uint32 numArraySlices,
                                                                           uint32 flags, RenderLayer::CEResourceState initialState,
                                                                           const RenderLayer::CEResourceData* initialData,
                                                                           const RenderLayer::CEClearValue& optimalClearValue) {
	const uint32 arraySlices = numArraySlices * TEXTURE_CUBE_FACE_COUNT;
	return CreateTexture<RenderLayer::CEDXTextureCubeArray>(format, size, size, arraySlices, numMips, 1, flags,
	                                                        initialState, initialData, optimalClearValue);
}

Main::RenderLayer::CETexture3D* CEDXManager::CreateTexture3D(RenderLayer::CEFormat format, uint32 width, uint32 height, uint32 depth, uint32 numMips,
                                                             uint32 flags, RenderLayer::CEResourceState initialState, const RenderLayer::CEResourceData* initialData,
                                                             const RenderLayer::CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTexture3D>(format, width, height, depth, numMips, 1, flags, initialState,
	                                                 initialData, optimizedClearValue);
}

Main::RenderLayer::CESamplerState* CEDXManager::CreateSamplerState(const RenderLayer::CESamplerStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEVertexBuffer* CEDXManager::CreateVertexBuffer(uint32 stride, uint32 numVertices, uint32 flags,
                                                                   RenderLayer::CEResourceState initialState, const RenderLayer::CEResourceData* initialData) {
	return nullptr;
}

Main::RenderLayer::CEIndexBuffer* CEDXManager::CreateIndexBuffer(RenderLayer::CEFormat format, uint32 numIndices, uint32 flags,
                                                                 RenderLayer::CEResourceState initialState, const RenderLayer::CEResourceData* initialData) {
	return nullptr;
}

Main::RenderLayer::CEConstantBuffer* CEDXManager::CreateConstantBuffer(uint32 size, uint32 flags, RenderLayer::CEResourceState initialState,
                                                                       const RenderLayer::CEResourceData* initialData) {
	return nullptr;
}

Main::RenderLayer::CEStructuredBuffer* CEDXManager::CreateStructuredBuffer(uint32 stride, uint32 numElements, uint32 flags,
                                                                           RenderLayer::CEResourceState initialState,
                                                                           const RenderLayer::CEResourceData* initialData) {
	return nullptr;
}

Main::RenderLayer::CERayTracingScene* CEDXManager::CreateRayTracingScene(uint32 flags, RenderLayer::CERayTracingGeometryInstance* instances,
                                                                         uint32 numInstances) {
	return nullptr;
}

Main::RenderLayer::CERayTracingGeometry* CEDXManager::CreateRayTracingGeometry(uint32 flags, RenderLayer::CEVertexBuffer* vertexBuffer,
                                                                               RenderLayer::CEIndexBuffer* indexBuffer) {
	return nullptr;
}

Main::RenderLayer::CEShaderResourceView* CEDXManager::CreateShaderResourceView(const RenderLayer::CEShaderResourceViewCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEUnorderedAccessView* CEDXManager::CreateUnorderedAccessView(const RenderLayer::CEUnorderedAccessViewCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CERenderTargetView* CEDXManager::CreateRenderTargetView(const RenderLayer::CERenderTargetViewCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEDepthStencilView* CEDXManager::CreateDepthStencilView(const RenderLayer::CEDepthStencilViewCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEComputeShader* CEDXManager::CreateComputeShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEVertexShader* CEDXManager::CreateVertexShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEHullShader* CEDXManager::CreateHullShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEDomainShader* CEDXManager::CreateDomainShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEGeometryShader* CEDXManager::CreateGeometryShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEAmplificationShader* CEDXManager::CreateAmplificationShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEPixelShader* CEDXManager::CreatePixelShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CERayGenShader* CEDXManager::CreateRayGenShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CERayAnyHitShader* CEDXManager::CreateRayAnyHitShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CERayClosestHitShader* CEDXManager::CreateClosestHitShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CERayMissShader* CEDXManager::CreateRayMissShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEDepthStencilState* CEDXManager::CreateDepthStencilState(const RenderLayer::CEDepthStencilStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CERasterizerState* CEDXManager::CreateRasterizerState(const RenderLayer::CERasterizerStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEBlendState* CEDXManager::CreateBlendState(const RenderLayer::CEBlendStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEInputLayoutState* CEDXManager::CreateInputLayout(const RenderLayer::CEInputLayoutStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEGraphicsPipelineState* CEDXManager::CreateGraphicsPipelineState(const RenderLayer::CEGraphicsPipelineStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEComputePipelineState* CEDXManager::CreateComputePipelineState(const RenderLayer::CEComputePipelineStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CERayTracingPipelineState* CEDXManager::CreatRayTracingPipelineState(
	const RenderLayer::CERayTracingPipelineStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEGPUProfiler* CEDXManager::CreateProfiler() {
	return nullptr;
}

Main::RenderLayer::CEViewport* CEDXManager::CreateViewport(Core::Platform::Generic::Window::CEWindow* window, uint32 width, uint32 height,
                                                           RenderLayer::CEFormat colorFormat, RenderLayer::CEFormat depthFormat) {
	return nullptr;
}

Main::RenderLayer::CEICommandContext* CEDXManager::GetDefaultCommandContext() {
	return nullptr;
}

void CEDXManager::CheckRayTracingSupport(Main::CERayTracingSupport& outSupport) {
}

void CEDXManager::CheckShadingRateSupport(Main::CEShadingRateSupport& outSupport) {
}


template <typename TCEDXTexture>
TCEDXTexture* CEDXManager::CreateTexture(RenderLayer::CEFormat format, uint32 sizeX, uint32 sizeY, uint32 sizeZ,
                                         uint32 numMips, uint32 numSamplers, uint32 flags, RenderLayer::CEResourceState initialState,
                                         const RenderLayer::CEResourceData* initialData, const RenderLayer::CEClearValue& optimalClearValue) {
	return nullptr;
}

template <typename TCEDXBuffer>
bool CEDXManager::FinishBufferResource(TCEDXBuffer* buffer,
                                       uint32 sizeInBytes,
                                       uint32 flags,
                                       RenderLayer::CEResourceState initialState,
                                       const RenderLayer::CEResourceData* initialData) {
	return false;
}
