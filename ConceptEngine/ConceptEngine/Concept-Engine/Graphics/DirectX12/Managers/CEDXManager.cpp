#include "CEDXManager.h"

#include "../RenderLayer/CEDXTexture.h"

#include "../RenderLayer/CEDXHelper.h"

#include "../../../Core/Application/CECore.h"

#include "../RenderLayer/CEDXRayTracing.h"

using namespace ConceptEngine::Graphics::DirectX12::Managers;
using namespace ConceptEngine::Graphics;

template <>
inline D3D12_RESOURCE_DIMENSION DirectX12::Managers::GetD3D12TextureResourceDimension<
	DirectX12::RenderLayer::CEDXTexture2D>() {
	return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

template <>
inline D3D12_RESOURCE_DIMENSION DirectX12::Managers::GetD3D12TextureResourceDimension<
	DirectX12::RenderLayer::CEDXTexture2DArray>() {
	return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

template <>
inline D3D12_RESOURCE_DIMENSION DirectX12::Managers::GetD3D12TextureResourceDimension<
	DirectX12::RenderLayer::CEDXTextureCube>() {
	return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

template <>
inline D3D12_RESOURCE_DIMENSION DirectX12::Managers::GetD3D12TextureResourceDimension<
	DirectX12::RenderLayer::CEDXTextureCubeArray>() {
	return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
}

template <>
inline D3D12_RESOURCE_DIMENSION DirectX12::Managers::GetD3D12TextureResourceDimension<
	DirectX12::RenderLayer::CEDXTexture3D>() {
	return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
}

template <typename TD3D12Texture>
inline bool DirectX12::Managers::IsTextureCube() {
	return false;
}

template <>
inline bool DirectX12::Managers::IsTextureCube<DirectX12::RenderLayer::CEDXTextureCube>() {
	return true;
}

template <>
inline bool DirectX12::Managers::IsTextureCube<DirectX12::RenderLayer::CEDXTextureCubeArray>() {
	return true;
}

CEDXManager::CEDXManager(): CEGraphicsManager(), Device(nullptr), DirectCommandContext(nullptr) {
}

CEDXManager::~CEDXManager() {
	DirectCommandContext.Reset();
	if (RootSignatureCache) {
		delete RootSignatureCache;
	}
	if (ResourceOfflineDescriptorHeap) {
		delete ResourceOfflineDescriptorHeap;
	}
	if (RenderTargetOfflineDescriptorHeap) {
		delete RenderTargetOfflineDescriptorHeap;
	}
	if (DepthStencilOfflineDescriptorHeap) {
		delete DepthStencilOfflineDescriptorHeap;
	}
	if (SamplerOfflineDescriptorHeap) {
		delete SamplerOfflineDescriptorHeap;
	}
	if (Device) {
		delete Device;
	}
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

Main::RenderLayer::CETexture2D* CEDXManager::CreateTexture2D(RenderLayer::CEFormat format,
                                                             uint32 width,
                                                             uint32 height,
                                                             uint32 numMips,
                                                             uint32 numSamples,
                                                             uint32 flags,
                                                             RenderLayer::CEResourceState initialState,
                                                             const RenderLayer::CEResourceData* initialData,
                                                             const RenderLayer::CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTexture2D>(format, width, height, 1, numMips, numSamples, flags, initialState,
	                                                 initialData, optimizedClearValue);
}

Main::RenderLayer::CETexture2DArray* CEDXManager::CreateTexture2DArray(RenderLayer::CEFormat format,
                                                                       uint32 width,
                                                                       uint32 height,
                                                                       uint32 numMips,
                                                                       uint32 numSamples,
                                                                       uint32 numArraySlices,
                                                                       uint32 flags,
                                                                       RenderLayer::CEResourceState initialState,
                                                                       const RenderLayer::CEResourceData* initialData,
                                                                       const RenderLayer::CEClearValue&
                                                                       optimizedClearValue) {
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
                                                                           uint32 flags,
                                                                           RenderLayer::CEResourceState initialState,
                                                                           const RenderLayer::CEResourceData*
                                                                           initialData,
                                                                           const RenderLayer::CEClearValue&
                                                                           optimalClearValue) {
	const uint32 arraySlices = numArraySlices * TEXTURE_CUBE_FACE_COUNT;
	return CreateTexture<RenderLayer::CEDXTextureCubeArray>(format, size, size, arraySlices, numMips, 1, flags,
	                                                        initialState, initialData, optimalClearValue);
}

Main::RenderLayer::CETexture3D* CEDXManager::CreateTexture3D(RenderLayer::CEFormat format,
                                                             uint32 width,
                                                             uint32 height,
                                                             uint32 depth,
                                                             uint32 numMips,
                                                             uint32 flags,
                                                             RenderLayer::CEResourceState initialState,
                                                             const RenderLayer::CEResourceData* initialData,
                                                             const RenderLayer::CEClearValue& optimizedClearValue) {
	return CreateTexture<RenderLayer::CEDXTexture3D>(format, width, height, depth, numMips, 1, flags, initialState,
	                                                 initialData, optimizedClearValue);
}

Main::RenderLayer::CESamplerState* CEDXManager::CreateSamplerState(
	const RenderLayer::CESamplerStateCreateInfo& createInfo) {
	D3D12_SAMPLER_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	desc.AddressU = RenderLayer::ConvertSamplerMode(createInfo.AddressU);
	desc.AddressV = RenderLayer::ConvertSamplerMode(createInfo.AddressV);
	desc.AddressW = RenderLayer::ConvertSamplerMode(createInfo.AddressW);
	desc.ComparisonFunc = RenderLayer::ConvertComparisonFunc(createInfo.ComparisonFunc);
	desc.Filter = RenderLayer::ConvertSamplerFilter(createInfo.Filter);
	desc.MaxAnisotropy = createInfo.MaxAnisotropy;
	desc.MaxLOD = createInfo.MaxLOD;
	desc.MinLOD = createInfo.MinLOD;
	desc.MipLODBias = createInfo.MipLODBias;

	Memory::CEMemory::Memcpy(desc.BorderColor, createInfo.BorderColor.Elements, sizeof(desc.BorderColor));

	Core::Common::CERef<RenderLayer::CEDXSamplerState> sampler = new RenderLayer::CEDXSamplerState(
		Device, SamplerOfflineDescriptorHeap);
	if (!sampler->Create(desc)) {
		return nullptr;
	}

	return sampler.ReleaseOwnership();
}

Main::RenderLayer::CEVertexBuffer* CEDXManager::CreateVertexBuffer(uint32 stride, uint32 numVertices, uint32 flags,
                                                                   RenderLayer::CEResourceState initialState,
                                                                   const RenderLayer::CEResourceData* initialData) {
	const uint32 sizeInBytes = numVertices * stride;
	Core::Common::CERef<RenderLayer::CEDXVertexBuffer> buffer = new RenderLayer::CEDXVertexBuffer(
		Device, numVertices, stride, flags);
	if (!FinishBufferResource<RenderLayer::CEDXVertexBuffer>(buffer.Get(), sizeInBytes, flags, initialState,
	                                                         initialData)) {
		CE_LOG_ERROR("[CEDXManager]: Failed to create VertexBuffer");
		return nullptr;
	}
	return buffer.ReleaseOwnership();
}

Main::RenderLayer::CEIndexBuffer* CEDXManager::CreateIndexBuffer(RenderLayer::CEIndexFormat format, uint32 numIndices,
                                                                 uint32 flags,
                                                                 RenderLayer::CEResourceState initialState,
                                                                 const RenderLayer::CEResourceData* initialData) {
	const uint32 sizeInBytes = numIndices * GetStrideFromIndexFormat(format);
	const uint32 alignedSizeInBytes = Math::CEMath::AlignUp<uint32>(sizeInBytes, sizeof(uint32));

	Core::Common::CERef<RenderLayer::CEDXIndexBuffer> buffer = new RenderLayer::CEDXIndexBuffer(
		Device, format, numIndices, flags);
	if (!FinishBufferResource<RenderLayer::CEDXIndexBuffer>(buffer.Get(), alignedSizeInBytes, flags, initialState,
	                                                        initialData)) {
		CE_LOG_ERROR("[CEDXManager]: Failed to create IndexBuffer");
		return nullptr;
	}
	return buffer.ReleaseOwnership();
}

Main::RenderLayer::CEConstantBuffer* CEDXManager::CreateConstantBuffer(uint32 size, uint32 flags,
                                                                       RenderLayer::CEResourceState initialState,
                                                                       const RenderLayer::CEResourceData* initialData) {
	Assert(!(flags & RenderLayer::BufferFlag_UAV) && !(flags & RenderLayer::BufferFlag_SRV));
	const uint32 alignedSizeInBytes = Math::CEMath::AlignUp<uint32>(
		size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	Core::Common::CERef<RenderLayer::CEDXConstantBuffer> buffer = new RenderLayer::CEDXConstantBuffer(
		Device, ResourceOfflineDescriptorHeap, size, flags);
	if (!FinishBufferResource<RenderLayer::CEDXConstantBuffer>(buffer.Get(), alignedSizeInBytes, flags, initialState,
	                                                           initialData)) {
		CE_LOG_ERROR("[CEDXManager]: Failed to create ConstantBuffer");
		return nullptr;
	}
	return buffer.ReleaseOwnership();
}

Main::RenderLayer::CEStructuredBuffer* CEDXManager::CreateStructuredBuffer(
	uint32 stride, uint32 numElements, uint32 flags,
	RenderLayer::CEResourceState initialState,
	const RenderLayer::CEResourceData* initialData) {
	const uint32 sizeInBytes = numElements * stride;
	Core::Common::CERef<RenderLayer::CEDXStructuredBuffer> buffer = new RenderLayer::CEDXStructuredBuffer(
		Device, numElements, stride, flags);
	if (!FinishBufferResource<RenderLayer::CEDXStructuredBuffer>(buffer.Get(), sizeInBytes, flags, initialState,
	                                                             initialData)) {
		CE_LOG_ERROR("[CEDXManager]: Failed to create StructuredBuffer");
		return nullptr;
	}
	return buffer.ReleaseOwnership();
}

Main::RenderLayer::CERayTracingScene* CEDXManager::CreateRayTracingScene(
	uint32 flags, RenderLayer::CERayTracingGeometryInstance* instances,
	uint32 numInstances) {
	Core::Common::CERef<RenderLayer::CEDXRayTracingScene> scene = new RenderLayer::CEDXRayTracingScene(Device, flags);
	DirectCommandContext->Execute([&scene, this, &instances, &numInstances] {
		if (!scene->Build(*DirectCommandContext, instances, numInstances, false)) {
			scene.Reset();
		}
	});

	return nullptr;
}

Main::RenderLayer::CERayTracingGeometry* CEDXManager::CreateRayTracingGeometry(
	uint32 flags, RenderLayer::CEVertexBuffer* vertexBuffer,
	RenderLayer::CEIndexBuffer* indexBuffer) {
	RenderLayer::CEDXVertexBuffer* dxVertexBuffer = static_cast<RenderLayer::CEDXVertexBuffer*>(vertexBuffer);
	RenderLayer::CEDXIndexBuffer* dxIndexBuffer = static_cast<RenderLayer::CEDXIndexBuffer*>(indexBuffer);

	Core::Common::CERef<RenderLayer::CEDXRayTracingGeometry> geometry = new RenderLayer::CEDXRayTracingGeometry(
		Device, flags);
	geometry->VertexBuffer = Core::Common::MakeSharedRef<RenderLayer::CEDXVertexBuffer>(dxVertexBuffer);
	geometry->IndexBuffer = Core::Common::MakeSharedRef<RenderLayer::CEDXIndexBuffer>(dxIndexBuffer);
	DirectCommandContext->Execute([this, &geometry] {
		if (!geometry->Build(*DirectCommandContext, false)) {
			geometry.Reset();
		}
	});

	return geometry.ReleaseOwnership();
}

Main::RenderLayer::CEShaderResourceView* CEDXManager::CreateShaderResourceView(
	const RenderLayer::CEShaderResourceViewCreateInfo& createInfo) {

	D3D12_SHADER_RESOURCE_VIEW_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	RenderLayer::CEDXResource* resource = nullptr;
	if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::Texture2D) {
		RenderLayer::CETexture2D* texture = createInfo.Texture2D.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsSRV() && createInfo.Texture2D.Format != RenderLayer::CEFormat::Unknown);

		desc.Format = RenderLayer::ConvertFormat(createInfo.Texture2D.Format);
		if (!texture->IsMultiSampled()) {
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = createInfo.Texture2D.NumMips;
			desc.Texture2D.MostDetailedMip = createInfo.Texture2D.Mip;
			desc.Texture2D.ResourceMinLODClamp = createInfo.Texture2D.MinMipsBias;
			desc.Texture2D.PlaneSlice = 0;
		}
		else {
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
		}
	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::Texture2DArray) {
		RenderLayer::CETexture2DArray* texture = createInfo.Texture2DArray.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsSRV() && createInfo.Texture2DArray.Format != RenderLayer::CEFormat::Unknown);

		desc.Format = RenderLayer::ConvertFormat(createInfo.Texture2DArray.Format);
		if (!texture->IsMultiSampled()) {
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipLevels = createInfo.Texture2DArray.NumMips;
			desc.Texture2DArray.MostDetailedMip = createInfo.Texture2DArray.Mip;
			desc.Texture2DArray.ResourceMinLODClamp = createInfo.Texture2DArray.MinMipBias;
			desc.Texture2DArray.ArraySize = createInfo.Texture2DArray.NumArraySlices;
			desc.Texture2DArray.FirstArraySlice = createInfo.Texture2DArray.ArraySlice;
			desc.Texture2DArray.PlaneSlice = 0;
		}
		else {
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
		}
	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::TextureCube) {

	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::TextureCubeArray) {

	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::Texture3D) {

	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::VertexBuffer) {

	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::IndexBuffer) {

	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::StructuredBuffer) {

	}

	Assert(resource != nullptr);
	Core::Common::CERef<RenderLayer::CEDXShaderResourceView> dxView = new RenderLayer::CEDXShaderResourceView(
		Device, ResourceOfflineDescriptorHeap);
	if (!dxView->Create()) {
		return nullptr;
	}

	if (dxView->CreateView(resource, desc)) {
		return dxView.ReleaseOwnership();
	}

	return nullptr;
}

Main::RenderLayer::CEUnorderedAccessView* CEDXManager::CreateUnorderedAccessView(
	const RenderLayer::CEUnorderedAccessViewCreateInfo& createInfo) {

	D3D12_UNORDERED_ACCESS_VIEW_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	RenderLayer::CEDXResource* resource = nullptr;
	if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::Texture2D) {

	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::Texture2DArray) {

	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::TextureCube) {

	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::TextureCubeArray) {

	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::Texture3D) {

	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::VertexBuffer) {

	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::IndexBuffer) {

	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::StructuredBuffer) {

	}

	Assert(resource != nullptr);
	Core::Common::CERef<RenderLayer::CEDXUnorderedAccessView> dxView = new RenderLayer::CEDXUnorderedAccessView(
		Device, ResourceOfflineDescriptorHeap);
	if (!dxView->Create()) {
		return nullptr;
	}

	if (dxView->CreateView(nullptr, resource, desc)) {
		return dxView.ReleaseOwnership();
	}

	return nullptr;
}

Main::RenderLayer::CERenderTargetView* CEDXManager::CreateRenderTargetView(
	const RenderLayer::CERenderTargetViewCreateInfo& createInfo) {

	D3D12_RENDER_TARGET_VIEW_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	RenderLayer::CEDXResource* resource = nullptr;
	if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::Texture2D) {

	}
	else if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::Texture2DArray) {

	}
	else if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::TextureCube) {

	}
	else if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::TextureCubeArray) {

	}
	else if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::Texture3D) {

	}

	Assert(resource != nullptr);
	Core::Common::CERef<RenderLayer::CEDXRenderTargetView> dxView = new RenderLayer::CEDXRenderTargetView(
		Device, ResourceOfflineDescriptorHeap);
	if (!dxView->Create()) {
		return nullptr;
	}

	if (dxView->CreateView(resource, desc)) {
		return dxView.ReleaseOwnership();
	}

	return nullptr;
}

Main::RenderLayer::CEDepthStencilView* CEDXManager::CreateDepthStencilView(
	const RenderLayer::CEDepthStencilViewCreateInfo& createInfo) {

	D3D12_DEPTH_STENCIL_VIEW_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	RenderLayer::CEDXResource* resource = nullptr;
	if (createInfo.Type == RenderLayer::CEDepthStencilViewCreateInfo::CEType::Texture2D) {

	}
	else if (createInfo.Type == RenderLayer::CEDepthStencilViewCreateInfo::CEType::Texture2DArray) {

	}
	else if (createInfo.Type == RenderLayer::CEDepthStencilViewCreateInfo::CEType::TextureCube) {

	}
	else if (createInfo.Type == RenderLayer::CEDepthStencilViewCreateInfo::CEType::TextureCubeArray) {

	}

	Assert(resource != nullptr);
	Core::Common::CERef<RenderLayer::CEDXDepthStencilView> dxView = new RenderLayer::CEDXDepthStencilView(
		Device, ResourceOfflineDescriptorHeap);
	if (!dxView->Create()) {
		return nullptr;
	}

	if (dxView->CreateView(resource, desc)) {
		return dxView.ReleaseOwnership();
	}

	return nullptr;
}

Main::RenderLayer::CEComputeShader*
CEDXManager::CreateComputeShader(const Core::Containers::CEArray<uint8>& shaderCode) {
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

Main::RenderLayer::CEGeometryShader* CEDXManager::CreateGeometryShader(
	const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEAmplificationShader* CEDXManager::CreateAmplificationShader(
	const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEPixelShader* CEDXManager::CreatePixelShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CERayGenShader* CEDXManager::CreateRayGenShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CERayAnyHitShader* CEDXManager::CreateRayAnyHitShader(
	const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CERayClosestHitShader* CEDXManager::CreateClosestHitShader(
	const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CERayMissShader*
CEDXManager::CreateRayMissShader(const Core::Containers::CEArray<uint8>& shaderCode) {
	return nullptr;
}

Main::RenderLayer::CEDepthStencilState* CEDXManager::CreateDepthStencilState(
	const RenderLayer::CEDepthStencilStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CERasterizerState* CEDXManager::CreateRasterizerState(
	const RenderLayer::CERasterizerStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEBlendState* CEDXManager::CreateBlendState(const RenderLayer::CEBlendStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEInputLayoutState* CEDXManager::CreateInputLayout(
	const RenderLayer::CEInputLayoutStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEGraphicsPipelineState* CEDXManager::CreateGraphicsPipelineState(
	const RenderLayer::CEGraphicsPipelineStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEComputePipelineState* CEDXManager::CreateComputePipelineState(
	const RenderLayer::CEComputePipelineStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CERayTracingPipelineState* CEDXManager::CreatRayTracingPipelineState(
	const RenderLayer::CERayTracingPipelineStateCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEGPUProfiler* CEDXManager::CreateProfiler() {
	return nullptr;
}

Main::RenderLayer::CEViewport* CEDXManager::CreateViewport(Core::Platform::Generic::Window::CEWindow* window,
                                                           uint32 width, uint32 height,
                                                           RenderLayer::CEFormat colorFormat,
                                                           RenderLayer::CEFormat depthFormat) {
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
                                         uint32 numMips, uint32 numSamplers, uint32 flags,
                                         RenderLayer::CEResourceState initialState,
                                         const RenderLayer::CEResourceData* initialData,
                                         const RenderLayer::CEClearValue& optimalClearValue) {
	Core::Common::CERef<TCEDXTexture> newTexture = new TCEDXTexture(Device,
	                                                                format,
	                                                                sizeX,
	                                                                sizeY,
	                                                                sizeZ,
	                                                                numMips,
	                                                                numSamplers,
	                                                                flags,
	                                                                optimalClearValue);

	D3D12_RESOURCE_DESC Desc;
	Memory::CEMemory::Memzero(&Desc);

	Desc.Dimension = GetD3D12TextureResourceDimension<TCEDXTexture>();
	Desc.Flags = RenderLayer::ConvertTextureFlags(flags);
	Desc.Format = RenderLayer::ConvertFormat(format);
	Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	Desc.Width = sizeX;
	Desc.Height = sizeY;
	Desc.DepthOrArraySize = (UINT16)sizeZ;
	Desc.MipLevels = (UINT16)numMips;
	Desc.Alignment = 0;
	Desc.SampleDesc.Count = numSamplers;

	if (numSamplers > 1) {
		const int32 quality = Device->GetMultiSampleQuality(Desc.Format, numSamplers);
		Desc.SampleDesc.Quality = quality - 1;
	}
	else {
		Desc.SampleDesc.Quality = 0;
	}

	D3D12_CLEAR_VALUE* clearValuePtr = nullptr;
	D3D12_CLEAR_VALUE clearValue;

	if (flags & RenderLayer::TextureFlag_RTV || flags & RenderLayer::TextureFlag_DSV) {
		clearValue.Format = optimalClearValue.GetFormat() != RenderLayer::CEFormat::Unknown
			                    ? RenderLayer::ConvertFormat(optimalClearValue.GetFormat())
			                    : Desc.Format;
		if (optimalClearValue.GetType() == RenderLayer::CEClearValue::CEType::DepthStencil) {
			clearValue.DepthStencil.Depth = optimalClearValue.AsDepthStencil().Depth;
			clearValue.DepthStencil.Stencil = optimalClearValue.AsDepthStencil().Stencil;
		}
		else if (optimalClearValue.GetType() == RenderLayer::CEClearValue::CEType::Color) {
			Memory::CEMemory::Memcpy(clearValue.Color, optimalClearValue.AsColor().Elements);
		}
		clearValuePtr = &clearValue;

	}

	Core::Common::CERef<RenderLayer::CEDXResource> resource = new RenderLayer::CEDXResource(
		Device, Desc, D3D12_HEAP_TYPE_DEFAULT);
	if (!resource->Create(D3D12_RESOURCE_STATE_COMMON, clearValuePtr)) {
		return nullptr;
	}

	newTexture->SetResource(resource.ReleaseOwnership());

	if (flags & RenderLayer::TextureFlag_SRV && !(flags & RenderLayer::TextureFlag_NoDefaultSRV)) {
		D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc;
		Memory::CEMemory::Memzero(&viewDesc);

		viewDesc.Format = RenderLayer::CastShaderResourceFormat(Desc.Format);
		viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (Desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
			if (sizeZ > 6 && IsTextureCube<TCEDXTexture>()) {
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
				viewDesc.TextureCubeArray.MipLevels = numMips;
				viewDesc.TextureCubeArray.MostDetailedMip = 0;
				viewDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
				viewDesc.TextureCubeArray.First2DArrayFace = 0;
				viewDesc.TextureCubeArray.NumCubes = sizeZ / TEXTURE_CUBE_FACE_COUNT;
			}
			else if (IsTextureCube<TCEDXTexture>()) {
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				viewDesc.TextureCube.MipLevels = numMips;
				viewDesc.TextureCube.MostDetailedMip = 0;
				viewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			}
			else if (sizeZ > 1) {
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				viewDesc.Texture2DArray.MipLevels = numMips;
				viewDesc.Texture2DArray.MostDetailedMip = 0;
				viewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
				viewDesc.Texture2DArray.PlaneSlice = 0;
				viewDesc.Texture2DArray.ArraySize = sizeZ;
				viewDesc.Texture2DArray.FirstArraySlice = 0;
			}
			else {
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				viewDesc.Texture2D.MipLevels = numMips;
				viewDesc.Texture2D.MostDetailedMip = 0;
				viewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				viewDesc.Texture2D.PlaneSlice = 0;
			}
		}
		else if (Desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D) {
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			viewDesc.Texture3D.MipLevels = numMips;
			viewDesc.Texture3D.MostDetailedMip = 0;
			viewDesc.Texture3D.ResourceMinLODClamp = 0.0f;
		}
		else {
			Assert(false);
			return nullptr;
		}

		Core::Common::CERef<RenderLayer::CEDXShaderResourceView> SRV = new RenderLayer::CEDXShaderResourceView(
			Device, ResourceOfflineDescriptorHeap);
		if (!SRV->Create()) {
			return nullptr;
		}

		if (!SRV->CreateView(newTexture->GetResource(), viewDesc)) {
			return nullptr;
		}

		newTexture->SetShaderResourceView(SRV.ReleaseOwnership());
	}

	const bool isTexture2D = (Desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) && (sizeZ == 1);
	if (flags & RenderLayer::TextureFlag_RTV && !(flags & RenderLayer::TextureFlag_NoDefaultRTV) && isTexture2D) {
		RenderLayer::CEDXTexture2D* newTexture2D = static_cast<RenderLayer::CEDXTexture2D*>(newTexture->AsTexture2D());

		D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
		Memory::CEMemory::Memzero(&viewDesc);

		viewDesc.Format = Desc.Format;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;
		viewDesc.Texture2D.PlaneSlice = 0;

		Core::Common::CERef<RenderLayer::CEDXRenderTargetView> RTV = new RenderLayer::CEDXRenderTargetView(
			Device, RenderTargetOfflineDescriptorHeap);
		if (!RTV->Create()) {
			return nullptr;
		}

		if (!RTV->CreateView(newTexture->GetResource(), viewDesc)) {
			return nullptr;
		}

		newTexture2D->SetRenderTargetView(RTV.ReleaseOwnership());
	}

	if (flags & RenderLayer::TextureFlag_DSV && !(flags & RenderLayer::TextureFlag_NoDefaultDSV) && isTexture2D) {
		RenderLayer::CEDXTexture2D* newTexture2D = static_cast<RenderLayer::CEDXTexture2D*>(newTexture->AsTexture2D());

		D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc;
		Memory::CEMemory::Memzero(&viewDesc);

		viewDesc.Format = Desc.Format;
		viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		Core::Common::CERef<RenderLayer::CEDXDepthStencilView> DSV = new RenderLayer::CEDXDepthStencilView(
			Device, DepthStencilOfflineDescriptorHeap);

		if (!DSV->Create()) {
			return nullptr;
		}

		if (!DSV->CreateView(newTexture->GetResource(), viewDesc)) {
			return nullptr;
		}

		newTexture2D->SetDepthStencilView(DSV.ReleaseOwnership());
	}

	if (flags & RenderLayer::TextureFlag_UAV && !(flags & RenderLayer::TextureFlag_NoDefaultUAV) && isTexture2D) {
		RenderLayer::CEDXTexture2D* newTexture2D = static_cast<RenderLayer::CEDXTexture2D*>(newTexture->AsTexture2D());

		D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc;
		Memory::CEMemory::Memzero(&viewDesc);

		viewDesc.Format = Desc.Format;
		viewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;
		viewDesc.Texture2D.PlaneSlice = 0;

		Core::Common::CERef<RenderLayer::CEDXUnorderedAccessView> UAV = new RenderLayer::CEDXUnorderedAccessView(
			Device, ResourceOfflineDescriptorHeap);
		if (!UAV->Create()) {
			return nullptr;
		}

		if (!UAV->CreateView(nullptr, newTexture->GetResource(), viewDesc)) {
			return nullptr;
		}

		newTexture2D->SetUnorderedAccessView(UAV.ReleaseOwnership());
	}

	if (initialData) {
		RenderLayer::CETexture2D* texture2D = newTexture->AsTexture2D();
		if (!texture2D) {
			return nullptr;
		}

		DirectCommandContext->Execute(
			[&texture2D, &sizeX, &sizeY, &sizeZ,&initialData, &initialState, this] {
				DirectCommandContext->TransitionTexture(texture2D, RenderLayer::CEResourceState::Common,
				                                        RenderLayer::CEResourceState::CopyDest);
				DirectCommandContext->UpdateTexture2D(texture2D, sizeX, sizeY, 0, initialData->GetData());

				DirectCommandContext->
					TransitionTexture(texture2D, RenderLayer::CEResourceState::CopyDest, initialState);
			});
	}
	else {
		if (initialState != RenderLayer::CEResourceState::Common) {

			DirectCommandContext->Execute(
				[&newTexture, &initialState, this] {
					DirectCommandContext->TransitionTexture(newTexture.Get(), RenderLayer::CEResourceState::Common,
					                                        initialState);
				});
		}
	}

	return newTexture.ReleaseOwnership();
}

template <typename TCEDXBuffer>
bool CEDXManager::FinishBufferResource(TCEDXBuffer* buffer,
                                       uint32 sizeInBytes,
                                       uint32 flags,
                                       RenderLayer::CEResourceState initialState,
                                       const RenderLayer::CEResourceData* initialData) {
	D3D12_RESOURCE_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags = RenderLayer::ConvertBufferFlags(flags);
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Width = sizeInBytes;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Alignment = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D12_HEAP_TYPE dxHeapType = D3D12_HEAP_TYPE_DEFAULT;
	D3D12_RESOURCE_STATES dxInitialState = D3D12_RESOURCE_STATE_COMMON;
	if (flags & RenderLayer::BufferFlag_Upload) {
		dxHeapType = D3D12_HEAP_TYPE_UPLOAD;
		dxInitialState = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	Core::Common::CERef<RenderLayer::CEDXResource> resource = new RenderLayer::CEDXResource(Device, desc, dxHeapType);
	if (!resource->Create(dxInitialState, nullptr)) {
		return false;
	}

	buffer->SetResource(resource.ReleaseOwnership());

	if (initialData) {
		if (buffer->IsUpload()) {
			Assert(buffer->GetSizeInBytes() <= sizeInBytes);

			void* hostData = buffer->Map(0, 0);
			if (!hostData) {
				return false;
			}

			Memory::CEMemory::Memcpy(hostData, initialData->GetData(), initialData->GetSizeInBytes());
			buffer->Unmap(0, 0);
		}
		else {
			DirectCommandContext->Execute([this, &buffer, &initialData, &initialState] {
				DirectCommandContext->TransitionBuffer(buffer, RenderLayer::CEResourceState::Common,
				                                       RenderLayer::CEResourceState::CopyDest);
				DirectCommandContext->UpdateBuffer(buffer, 0, initialData->GetSizeInBytes(), initialData->GetData());

				DirectCommandContext->TransitionBuffer(buffer, RenderLayer::CEResourceState::CopyDest, initialState);
			});
		}
	}
	else {
		if (initialState != RenderLayer::CEResourceState::Common && !buffer->IsUpload()) {
			DirectCommandContext->Execute([this, &initialState, &buffer] {
				DirectCommandContext->TransitionBuffer(buffer, RenderLayer::CEResourceState::Common, initialState);
			});
		}
	}

	return true;
}
