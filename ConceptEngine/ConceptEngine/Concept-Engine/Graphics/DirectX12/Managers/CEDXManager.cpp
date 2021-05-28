#include "CEDXManager.h"

#include "../RenderLayer/CEDXTexture.h"

#include "../RenderLayer/CEDXHelper.h"

#include "../../../Core/Application/CECore.h"

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
	return nullptr;
}

Main::RenderLayer::CEVertexBuffer* CEDXManager::CreateVertexBuffer(uint32 stride, uint32 numVertices, uint32 flags,
                                                                   RenderLayer::CEResourceState initialState,
                                                                   const RenderLayer::CEResourceData* initialData) {
	return nullptr;
}

Main::RenderLayer::CEIndexBuffer* CEDXManager::CreateIndexBuffer(RenderLayer::CEFormat format, uint32 numIndices,
                                                                 uint32 flags,
                                                                 RenderLayer::CEResourceState initialState,
                                                                 const RenderLayer::CEResourceData* initialData) {
	return nullptr;
}

Main::RenderLayer::CEConstantBuffer* CEDXManager::CreateConstantBuffer(uint32 size, uint32 flags,
                                                                       RenderLayer::CEResourceState initialState,
                                                                       const RenderLayer::CEResourceData* initialData) {
	return nullptr;
}

Main::RenderLayer::CEStructuredBuffer* CEDXManager::CreateStructuredBuffer(
	uint32 stride, uint32 numElements, uint32 flags,
	RenderLayer::CEResourceState initialState,
	const RenderLayer::CEResourceData* initialData) {
	return nullptr;
}

Main::RenderLayer::CERayTracingScene* CEDXManager::CreateRayTracingScene(
	uint32 flags, RenderLayer::CERayTracingGeometryInstance* instances,
	uint32 numInstances) {
	return nullptr;
}

Main::RenderLayer::CERayTracingGeometry* CEDXManager::CreateRayTracingGeometry(
	uint32 flags, RenderLayer::CEVertexBuffer* vertexBuffer,
	RenderLayer::CEIndexBuffer* indexBuffer) {
	return nullptr;
}

Main::RenderLayer::CEShaderResourceView* CEDXManager::CreateShaderResourceView(
	const RenderLayer::CEShaderResourceViewCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEUnorderedAccessView* CEDXManager::CreateUnorderedAccessView(
	const RenderLayer::CEUnorderedAccessViewCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CERenderTargetView* CEDXManager::CreateRenderTargetView(
	const RenderLayer::CERenderTargetViewCreateInfo& createInfo) {
	return nullptr;
}

Main::RenderLayer::CEDepthStencilView* CEDXManager::CreateDepthStencilView(
	const RenderLayer::CEDepthStencilViewCreateInfo& createInfo) {
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
	//TODO: Add Constructor
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

	return newTexture.ReleaseOwnership();
}

template <typename TCEDXBuffer>
bool CEDXManager::FinishBufferResource(TCEDXBuffer* buffer,
                                       uint32 sizeInBytes,
                                       uint32 flags,
                                       RenderLayer::CEResourceState initialState,
                                       const RenderLayer::CEResourceData* initialData) {
	return false;
}
