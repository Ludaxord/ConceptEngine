#include "CEDXManager.h"

#include "../RenderLayer/CEDXShaderCompiler.h"
#include "../RenderLayer/CEDXTexture.h"

#include "../../../../Core/Application/CECore.h"

using namespace ConceptEngine::Graphics::DirectX12::Modules::Managers;
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

CETexture2D* CEDXManager::CreateTexture2D(CEFormat format, uint32 width, uint32 height, uint32 numMips,
                                          uint32 numSamples, uint32 flags, CEResourceState initialState,
                                          const CEResourceData* initialData,
                                          const CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTexture2D>(format, width, height, 1, numMips, numSamples, flags, initialState,
	                                                 initialData, optimizedClearValue);
}

CETexture2DArray* CEDXManager::CreateTexture2DArray(CEFormat format, uint32 width, uint32 height, uint32 numMips,
                                                    uint32 numSamples, uint32 numArraySlices, uint32 flags,
                                                    CEResourceState initialState,
                                                    const CEResourceData* initialData,
                                                    const CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTexture2DArray>(format, width, height, numArraySlices, numMips, numSamples,
	                                                      flags, initialState, initialData, optimizedClearValue);
}

CETextureCube* CEDXManager::CreateTextureCube(CEFormat format,
                                              uint32 size,
                                              uint32 numMips,
                                              uint32 flags,
                                              CEResourceState initialState,
                                              const CEResourceData* initialData,
                                              const CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTextureCube>(format, size, size, TEXTURE_CUBE_FACE_COUNT, numMips, 1,
	                                                   flags, initialState, initialData, optimizedClearValue);
}

CETextureCubeArray* CEDXManager::CreateTextureCubeArray(CEFormat format,
                                                        uint32 size,
                                                        uint32 numMips,
                                                        uint32 numArraySlices,
                                                        uint32 flags, CEResourceState initialState,
                                                        const CEResourceData* initialData,
                                                        const CEClearValue& optimalClearValue) {
	const uint32 arraySlices = numArraySlices * TEXTURE_CUBE_FACE_COUNT;
	return CreateTexture<RenderLayer::CEDXTextureCubeArray>(format, size, size, arraySlices, numMips, 1, flags,
	                                                        initialState, initialData, optimalClearValue);
}

CETexture3D* CEDXManager::CreateTexture3D(CEFormat format, uint32 width, uint32 height, uint32 depth, uint32 numMips,
                                          uint32 flags, CEResourceState initialState, const CEResourceData* initialData,
                                          const CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTexture3D>(format, width, height, depth, numMips, 1, flags, initialState,
	                                                 initialData, optimizedClearValue);
}

CESamplerState* CEDXManager::CreateSamplerState(const CESamplerStateCreateInfo& createInfo) {
	return nullptr;
}

CEVertexBuffer* CEDXManager::CreateVertexBuffer(uint32 stride, uint32 numVertices, uint32 flags,
                                                CEResourceState initialState, const CEResourceData* initialData) {
	return nullptr;
}

CEIndexBuffer* CEDXManager::CreateIndexBuffer(CEFormat format, uint32 numIndices, uint32 flags,
                                              CEResourceState initialState, const CEResourceData* initialData) {
	return nullptr;
}

CEConstantBuffer* CEDXManager::CreateConstantBuffer(uint32 size, uint32 flags, CEResourceState initialState,
                                                    const CEResourceData* initialData) {
	return nullptr;
}

CEStructuredBuffer* CEDXManager::CreateStructuredBuffer(uint32 stride, uint32 numElements, uint32 flags,
                                                        CEResourceState initialState,
                                                        const CEResourceData* initialData) {
	return nullptr;
}

CERayTracingScene* CEDXManager::CreateRayTracingScene(uint32 flags, CERayTracingGeometryInstance* instances,
                                                      uint32 numInstances) {
	return nullptr;
}

CERayTracingGeometry* CEDXManager::CreateRayTracingGeometry(uint32 flags, CEVertexBuffer* vertexBuffer,
                                                            CEIndexBuffer* indexBuffer) {
	return nullptr;
}

CEShaderResourceView* CEDXManager::CreateShaderResourceView(const CEShaderResourceViewCreateInfo& createInfo) {
	return nullptr;
}

CEUnorderedAccessView* CEDXManager::CreateUnorderedAccessView(const CEUnorderedAccessViewCreateInfo& createInfo) {
	return nullptr;
}

CERenderTargetView* CEDXManager::CreateRenderTargetView(const CERenderTargetViewCreateInfo& createInfo) {
	return nullptr;
}

CEDepthStencilView* CEDXManager::CreateDepthStencilView(const CEDepthStencilViewCreateInfo& createInfo) {
	return nullptr;
}

CEComputeShader* CEDXManager::CreateComputeShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CEVertexShader* CEDXManager::CreateVertexShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CEHullShader* CEDXManager::CreateHullShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CEDomainShader* CEDXManager::CreateDomainShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CEGeometryShader* CEDXManager::CreateGeometryShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CEAmplificationShader* CEDXManager::CreateAmplificationShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CEPixelShader* CEDXManager::CreatePixelShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CERayGenShader* CEDXManager::CreateRayGenShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CERayGenHitShader* CEDXManager::CreateRayGenHitShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CEClosestHitShader* CEDXManager::CreateClosestHitShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CERayMissShader* CEDXManager::CreateRayMissShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

CEDepthStencilState* CEDXManager::CreateDepthStencilState(const CEDepthStencilStateCreateInfo& createInfo) {
	return nullptr;
}

CERasterizerState* CEDXManager::CreateRasterizerState(const CERasterizerStateCreateInfo& createInfo) {
	return nullptr;
}

CEBlendState* CEDXManager::CreateBlendState(const CEBlendStateCreateInfo& createInfo) {
	return nullptr;
}

CEInputLayoutState* CEDXManager::CreateInputLayout(const CEInputLayoutStateCreateInfo& createInfo) {
	return nullptr;
}

CEGraphicsPipelineState* CEDXManager::CreateGraphicsPipelineState(const CEGraphicsPipelineStateCreateInfo& createInfo) {
	return nullptr;
}

CEComputePipelineState* CEDXManager::CreateComputePipelineState(const CEComputePipelineStateCreateInfo& createInfo) {
	return nullptr;
}

CERayTracingPipelineState* CEDXManager::CreatRayTracingPipelineState(
	const CERayTracingPipelineStateCreateInfo& createInfo) {
	return nullptr;
}

CEGPUProfiler* CEDXManager::CreateProfiler() {
	return nullptr;
}

CEViewport* CEDXManager::CreateViewport(Core::Platform::Generic::Window::CEWindow* window, uint32 width, uint32 height,
                                        CEFormat colorFormat, CEFormat depthFormat) {
	return nullptr;
}

CEICommandContext* CEDXManager::GetDefaultCommandContext() {
	return nullptr;
}

void CEDXManager::CheckRayTracingSupport(Main::CERayTracingSupport& outSupport) {
}

void CEDXManager::CheckShadingRateSupport(Main::CEShadingRateSupport& outSupport) {
}


template <typename TCEDXTexture>
TCEDXTexture* CEDXManager::CreateTexture(CEFormat format, uint32 sizeX, uint32 sizeY, uint32 sizeZ,
                                         uint32 numMips, uint32 numSamplers, uint32 flags, CEResourceState initialState,
                                         const CEResourceData* initialData, const CEClearValue& optimalClearValue) {
	return nullptr;
}

template <typename TCEDXBuffer>
bool CEDXManager::FinishBufferResource(TCEDXBuffer* buffer,
                                       uint32 sizeInBytes,
                                       uint32 flags,
                                       CEResourceState initialState,
                                       const CEResourceData* initialData) {
	return false;
}
