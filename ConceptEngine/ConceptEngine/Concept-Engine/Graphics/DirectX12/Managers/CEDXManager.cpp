#include "CEDXManager.h"

#include "../RenderLayer/CEDXTexture.h"
#include "../RenderLayer/CEDXRayTracing.h"
#include "../RenderLayer/CEDXViewport.h"

#include "../../../Core/Application/CECore.h"
#include "../../../Core/Platform/Windows/Window/CEWindowsWindow.h"
#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"

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

CEDXManager::CEDXManager(): CEGraphicsManager(),
                            Device(nullptr),
                            DirectCommandContext(nullptr),
                            RootSignatureCache(nullptr),
                            Aftermath(nullptr) {
}

CEDXManager::~CEDXManager() {
	DirectCommandContext.Reset();

	SafeDelete(RootSignatureCache);

	SafeDelete(ResourceOfflineDescriptorHeap);
	SafeDelete(RenderTargetOfflineDescriptorHeap);
	SafeDelete(DepthStencilOfflineDescriptorHeap);
	SafeDelete(SamplerOfflineDescriptorHeap);
	SafeDelete(SamplerOfflineDescriptorHeap);
	SafeDelete(Device);

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
		CEDebug::DebugBreak();
		return false;
	}

	if (Device->IsNsightAftermathEnabled()) {
		if (!Aftermath->Create(Device)) {
			return false;
		}
	}

	RootSignatureCache = new RenderLayer::CEDXRootSignatureCache(Device);
	if (!RootSignatureCache->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	ResourceOfflineDescriptorHeap = new RenderLayer::CEDXOfflineDescriptorHeap(
		Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	if (!ResourceOfflineDescriptorHeap->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	RenderTargetOfflineDescriptorHeap = new RenderLayer::CEDXOfflineDescriptorHeap(
		Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	if (!RenderTargetOfflineDescriptorHeap->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	DepthStencilOfflineDescriptorHeap = new RenderLayer::CEDXOfflineDescriptorHeap(
		Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	if (!DepthStencilOfflineDescriptorHeap->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	SamplerOfflineDescriptorHeap = new RenderLayer::CEDXOfflineDescriptorHeap(
		Device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	if (!SamplerOfflineDescriptorHeap->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	DirectCommandContext = new RenderLayer::CEDXCommandContext(Device);
	if (!DirectCommandContext->Create()) {
		CEDebug::DebugBreak();
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

	CERef<RenderLayer::CEDXSamplerState> sampler = new RenderLayer::CEDXSamplerState(
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
	CERef<RenderLayer::CEDXVertexBuffer> buffer = new RenderLayer::CEDXVertexBuffer(Device, numVertices, stride, flags);
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

	CERef<RenderLayer::CEDXIndexBuffer> buffer = new RenderLayer::CEDXIndexBuffer(
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

	CERef<RenderLayer::CEDXConstantBuffer> buffer = new RenderLayer::CEDXConstantBuffer(
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
	CERef<RenderLayer::CEDXStructuredBuffer> buffer = new RenderLayer::CEDXStructuredBuffer(
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
	CERef<RenderLayer::CEDXRayTracingScene> scene = new RenderLayer::CEDXRayTracingScene(Device, flags);
	DirectCommandContext->Execute([&scene, this, &instances, &numInstances] {
		if (!scene->Build(*DirectCommandContext, instances, numInstances, false)) {
			scene.Reset();
		}
	});

	return scene.ReleaseOwnership();
}

Main::RenderLayer::CERayTracingGeometry* CEDXManager::CreateRayTracingGeometry(
	uint32 flags, RenderLayer::CEVertexBuffer* vertexBuffer,
	RenderLayer::CEIndexBuffer* indexBuffer) {
	RenderLayer::CEDXVertexBuffer* dxVertexBuffer = static_cast<RenderLayer::CEDXVertexBuffer*>(vertexBuffer);
	RenderLayer::CEDXIndexBuffer* dxIndexBuffer = static_cast<RenderLayer::CEDXIndexBuffer*>(indexBuffer);

	CERef<RenderLayer::CEDXRayTracingGeometry> geometry = new RenderLayer::CEDXRayTracingGeometry(
		Device, flags);
	geometry->VertexBuffer = MakeSharedRef<RenderLayer::CEDXVertexBuffer>(dxVertexBuffer);
	geometry->IndexBuffer = MakeSharedRef<RenderLayer::CEDXIndexBuffer>(dxIndexBuffer);
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
			desc.Texture2DMSArray.ArraySize = createInfo.Texture2DArray.NumArraySlices;
			desc.Texture2DMSArray.FirstArraySlice = createInfo.Texture2DArray.ArraySlice;
		}
	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::TextureCube) {
		RenderLayer::CETextureCube* texture = createInfo.TextureCube.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsSRV() && createInfo.TextureCube.Format != RenderLayer::CEFormat::Unknown);

		desc.Format = RenderLayer::ConvertFormat(createInfo.TextureCube.Format);
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = createInfo.TextureCube.NumMips;
		desc.TextureCube.MostDetailedMip = createInfo.TextureCube.Mip;
		desc.TextureCube.ResourceMinLODClamp = createInfo.TextureCube.MinMipBias;
	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::TextureCubeArray) {
		RenderLayer::CETextureCubeArray* texture = createInfo.TextureCubeArray.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsSRV() && createInfo.TextureCubeArray.Format != RenderLayer::CEFormat::Unknown);

		//TODO: Check if it is correct???
		desc.Format = RenderLayer::ConvertFormat(createInfo.Texture2D.Format);
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
		desc.TextureCubeArray.MipLevels = createInfo.TextureCubeArray.NumMips;
		desc.TextureCubeArray.MostDetailedMip = createInfo.TextureCubeArray.Mip;
		desc.TextureCubeArray.ResourceMinLODClamp = createInfo.TextureCubeArray.MinMipBias;
		desc.TextureCubeArray.First2DArrayFace = createInfo.TextureCubeArray.ArraySlice * TEXTURE_CUBE_FACE_COUNT;
		desc.TextureCubeArray.NumCubes = createInfo.TextureCubeArray.NumArraySlices;

	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::Texture3D) {
		RenderLayer::CETexture3D* texture = createInfo.Texture3D.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsSRV() && createInfo.Texture3D.Format != RenderLayer::CEFormat::Unknown);

		desc.Format = RenderLayer::ConvertFormat(createInfo.Texture3D.Format);
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MipLevels = createInfo.Texture3D.NumMips;
		desc.Texture3D.MostDetailedMip = createInfo.Texture3D.Mip;
		desc.Texture3D.ResourceMinLODClamp = createInfo.Texture3D.MinMipBias;
	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::VertexBuffer) {
		RenderLayer::CEVertexBuffer* buffer = createInfo.VertexBuffer.Buffer;
		RenderLayer::CEDXBaseBuffer* dxBuffer = RenderLayer::CEDXBufferCast(buffer);
		resource = dxBuffer->GetResource();

		Assert(buffer->IsSRV());

		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = createInfo.VertexBuffer.FirstVertex;
		desc.Buffer.NumElements = createInfo.VertexBuffer.NumVertices;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		desc.Buffer.StructureByteStride = buffer->GetStride();
	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::IndexBuffer) {
		RenderLayer::CEIndexBuffer* buffer = createInfo.IndexBuffer.Buffer;
		RenderLayer::CEDXBaseBuffer* dxBuffer = RenderLayer::CEDXBufferCast(buffer);
		resource = dxBuffer->GetResource();

		Assert(buffer->IsSRV());
		Assert(buffer->GetFormat() != RenderLayer::CEIndexFormat::uint16);

		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = createInfo.IndexBuffer.FirstIndex;
		desc.Buffer.NumElements = createInfo.IndexBuffer.NumIndices;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
		desc.Buffer.StructureByteStride = 0;
	}
	else if (createInfo.Type == RenderLayer::CEShaderResourceViewCreateInfo::CEType::StructuredBuffer) {
		RenderLayer::CEStructuredBuffer* buffer = createInfo.StructuredBuffer.Buffer;
		RenderLayer::CEDXBaseBuffer* dxBuffer = RenderLayer::CEDXBufferCast(buffer);
		resource = dxBuffer->GetResource();

		Assert(buffer->IsSRV());

		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = createInfo.StructuredBuffer.FirstElement;
		desc.Buffer.NumElements = createInfo.StructuredBuffer.NumElements;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		desc.Buffer.StructureByteStride = buffer->GetStride();
	}

	Assert(resource != nullptr);
	CERef<RenderLayer::CEDXShaderResourceView> dxView = new RenderLayer::CEDXShaderResourceView(
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
		RenderLayer::CETexture2D* texture = createInfo.Texture2D.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsUAV() && createInfo.Texture2D.Format != RenderLayer::CEFormat::Unknown);

		desc.Format = RenderLayer::ConvertFormat(createInfo.Texture2D.Format);
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = createInfo.Texture2D.Mip;
		desc.Texture2D.PlaneSlice = 0;
	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::Texture2DArray) {
		RenderLayer::CETexture2DArray* texture = createInfo.Texture2DArray.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsUAV() && createInfo.Texture2DArray.Format != RenderLayer::CEFormat::Unknown);

		desc.Format = RenderLayer::ConvertFormat(createInfo.Texture2DArray.Format);
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = createInfo.Texture2DArray.Mip;
		desc.Texture2DArray.ArraySize = createInfo.Texture2DArray.NumArraySlices;
		desc.Texture2DArray.FirstArraySlice = createInfo.Texture2DArray.ArraySlice;
		desc.Texture2DArray.PlaneSlice = 0;
	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::TextureCube) {
		RenderLayer::CETextureCube* texture = createInfo.TextureCube.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsUAV() && createInfo.TextureCube.Format != RenderLayer::CEFormat::Unknown);

		desc.Format = RenderLayer::ConvertFormat(createInfo.TextureCube.Format);
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = createInfo.TextureCube.Mip;
		desc.Texture2DArray.ArraySize = TEXTURE_CUBE_FACE_COUNT;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.PlaneSlice = 0;

	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::TextureCubeArray) {
		RenderLayer::CETextureCubeArray* texture = createInfo.TextureCubeArray.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsUAV() && createInfo.TextureCubeArray.Format != RenderLayer::CEFormat::Unknown);

		desc.Format = RenderLayer::ConvertFormat(createInfo.TextureCubeArray.Format);
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = createInfo.TextureCubeArray.Mip;
		desc.Texture2DArray.ArraySize = createInfo.TextureCubeArray.NumArraySlices * TEXTURE_CUBE_FACE_COUNT;
		desc.Texture2DArray.FirstArraySlice = createInfo.TextureCubeArray.ArraySlice * TEXTURE_CUBE_FACE_COUNT;
		desc.Texture2DArray.PlaneSlice = 0;
	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::Texture3D) {
		RenderLayer::CETexture3D* texture = createInfo.Texture3D.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsUAV() && createInfo.Texture3D.Format != RenderLayer::CEFormat::Unknown);

		desc.Format = RenderLayer::ConvertFormat(createInfo.Texture3D.Format);
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MipSlice = createInfo.Texture3D.Mip;
		desc.Texture3D.FirstWSlice = createInfo.Texture3D.DepthSlice;
		desc.Texture3D.WSize = createInfo.Texture3D.NumDepthSlices;
	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::VertexBuffer) {
		RenderLayer::CEVertexBuffer* buffer = createInfo.VertexBuffer.Buffer;
		RenderLayer::CEDXBaseBuffer* dxBuffer = RenderLayer::CEDXBufferCast(buffer);
		resource = dxBuffer->GetResource();

		Assert(buffer->IsUAV());

		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = createInfo.VertexBuffer.FirstVertex;
		desc.Buffer.NumElements = createInfo.VertexBuffer.NumVertices;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		desc.Buffer.StructureByteStride = buffer->GetStride();
	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::IndexBuffer) {
		RenderLayer::CEIndexBuffer* buffer = createInfo.IndexBuffer.Buffer;
		RenderLayer::CEDXBaseBuffer* dxBuffer = RenderLayer::CEDXBufferCast(buffer);
		resource = dxBuffer->GetResource();

		Assert(buffer->IsUAV());

		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = createInfo.IndexBuffer.FirstIndex;
		desc.Buffer.NumElements = createInfo.IndexBuffer.NumIndices;

		Assert(buffer->GetFormat() != RenderLayer::CEIndexFormat::uint16);

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.StructureByteStride = 0;
	}
	else if (createInfo.Type == RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::StructuredBuffer) {
		RenderLayer::CEStructuredBuffer* buffer = createInfo.StructuredBuffer.Buffer;
		RenderLayer::CEDXBaseBuffer* dxBuffer = RenderLayer::CEDXBufferCast(buffer);
		resource = dxBuffer->GetResource();

		Assert(buffer->IsUAV());

		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = createInfo.StructuredBuffer.FirstElement;
		desc.Buffer.NumElements = createInfo.StructuredBuffer.NumElements;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		desc.Buffer.StructureByteStride = buffer->GetStride();
	}

	CERef<RenderLayer::CEDXUnorderedAccessView> dxView = new RenderLayer::CEDXUnorderedAccessView(
		Device, ResourceOfflineDescriptorHeap);
	if (!dxView->Create()) {
		return nullptr;
	}

	Assert(resource != nullptr);

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

	desc.Format = RenderLayer::ConvertFormat(createInfo.Format);
	Assert(createInfo.Format != RenderLayer::CEFormat::Unknown);

	if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::Texture2D) {
		RenderLayer::CETexture2D* texture = createInfo.Texture2D.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsRTV());

		if (texture->IsMultiSampled()) {
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = createInfo.Texture2D.Mip;
			desc.Texture2D.PlaneSlice = 0;
		}
		else {
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
		}
	}
	else if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::Texture2DArray) {
		RenderLayer::CETexture2DArray* texture = createInfo.Texture2DArray.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsRTV());

		if (texture->IsMultiSampled()) {
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipSlice = createInfo.Texture2DArray.Mip;
			desc.Texture2DArray.ArraySize = createInfo.Texture2DArray.NumArraySlices;
			desc.Texture2DArray.FirstArraySlice = createInfo.Texture2DArray.ArraySlice;
			desc.Texture2DArray.PlaneSlice = 0;
		}
		else {
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
			desc.Texture2DMSArray.ArraySize = createInfo.Texture2DArray.NumArraySlices;
			desc.Texture2DMSArray.FirstArraySlice = createInfo.Texture2DArray.ArraySlice;
		}
	}
	else if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::TextureCube) {
		RenderLayer::CETextureCube* texture = createInfo.TextureCube.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsRTV());

		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = createInfo.TextureCubeArray.Mip;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.FirstArraySlice = RenderLayer::GetCubeFaceIndex(createInfo.TextureCube.CubeFace);
		desc.Texture2DArray.PlaneSlice = 0;
	}
	else if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::TextureCubeArray) {
		RenderLayer::CETextureCube* texture = createInfo.TextureCubeArray.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsRTV());

		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = createInfo.TextureCubeArray.Mip;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.FirstArraySlice = createInfo.TextureCubeArray.ArraySlice * TEXTURE_CUBE_FACE_COUNT +
			RenderLayer::GetCubeFaceIndex(createInfo.TextureCube.CubeFace);
		desc.Texture2DArray.PlaneSlice = 0;
	}
	else if (createInfo.Type == RenderLayer::CERenderTargetViewCreateInfo::CEType::Texture3D) {
		RenderLayer::CETexture3D* texture = createInfo.Texture3D.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsRTV());

		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MipSlice = createInfo.Texture3D.Mip;
		desc.Texture3D.FirstWSlice = createInfo.Texture3D.DepthSlice;
		desc.Texture3D.WSize = createInfo.Texture3D.NumDepthSlices;
	}

	Assert(resource != nullptr);
	CERef<RenderLayer::CEDXRenderTargetView> dxView = new RenderLayer::CEDXRenderTargetView(
		Device, ResourceOfflineDescriptorHeap);
	if (!dxView->Create()) {
		return nullptr;
	}

	if (dxView->CreateView(resource, desc)) {
		return dxView.ReleaseOwnership();
	}

	return nullptr;
}

CEDepthStencilView* CEDXManager::CreateDepthStencilView(const CEDepthStencilViewCreateInfo& createInfo) {

	D3D12_DEPTH_STENCIL_VIEW_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	RenderLayer::CEDXResource* resource = nullptr;

	desc.Format = RenderLayer::ConvertFormat(createInfo.Format);
	Assert(createInfo.Format != RenderLayer::CEFormat::Unknown);

	if (createInfo.Type == CEDepthStencilViewCreateInfo::CEType::Texture2D) {
		RenderLayer::CETexture2D* texture = createInfo.Texture2D.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsDSV());

		if (texture->IsMultiSampled()) {
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = createInfo.Texture2D.Mip;
		}
		else {
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		}
	}
	else if (createInfo.Type == RenderLayer::CEDepthStencilViewCreateInfo::CEType::Texture2DArray) {
		RenderLayer::CETexture2DArray* texture = createInfo.Texture2DArray.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsDSV());

		if (texture->IsMultiSampled()) {
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipSlice = createInfo.Texture2DArray.Mip;
			desc.Texture2DArray.ArraySize = createInfo.Texture2DArray.NumArraySlices;
			desc.Texture2DArray.FirstArraySlice = createInfo.Texture2DArray.ArraySlice;
		}
		else {
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
			desc.Texture2DMSArray.ArraySize = createInfo.Texture2DArray.NumArraySlices;
			desc.Texture2DMSArray.FirstArraySlice = createInfo.Texture2DArray.ArraySlice;
		}

	}
	else if (createInfo.Type == RenderLayer::CEDepthStencilViewCreateInfo::CEType::TextureCube) {
		RenderLayer::CETextureCube* texture = createInfo.TextureCube.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsDSV());

		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = createInfo.TextureCube.Mip;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.FirstArraySlice = RenderLayer::GetCubeFaceIndex(createInfo.TextureCube.CubeFace);
	}
	else if (createInfo.Type == RenderLayer::CEDepthStencilViewCreateInfo::CEType::TextureCubeArray) {
		CETextureCubeArray* texture = createInfo.TextureCubeArray.Texture;
		RenderLayer::CEDXBaseTexture* dxTexture = RenderLayer::TextureCast(texture);
		resource = dxTexture->GetResource();

		Assert(texture->IsDSV());

		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = createInfo.TextureCubeArray.Mip;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.FirstArraySlice = createInfo.TextureCubeArray.ArraySlice * TEXTURE_CUBE_FACE_COUNT +
			GetCubeFaceIndex(createInfo.TextureCube.CubeFace);
	}

	CERef<RenderLayer::CEDXDepthStencilView> dxView = new RenderLayer::CEDXDepthStencilView(
		Device, DepthStencilOfflineDescriptorHeap);
	if (!dxView->Create()) {
		return nullptr;
	}

	if (dxView->CreateView(resource, desc)) {
		return dxView.ReleaseOwnership();
	}

	return nullptr;
}

CEComputeShader* CEDXManager::CreateComputeShader(const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXComputeShader> shader = new RenderLayer::CEDXComputeShader(Device, shaderCode);
	if (!shader->Create()) {
		CE_LOG_ERROR("[CEDXManager] Failed to Create Compute Shader...")
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

CEVertexShader* CEDXManager::CreateVertexShader(const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXVertexShader> shader = new RenderLayer::CEDXVertexShader(Device, shaderCode);
	if (!RenderLayer::CEDXBaseShader::GetShaderReflection(shader.Get())) {
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CEHullShader* CEDXManager::CreateHullShader(const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXHullShader> shader = new RenderLayer::CEDXHullShader(Device, shaderCode);
	if (!shader->Create()) {
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CEDomainShader* CEDXManager::CreateDomainShader(const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXDomainShader> shader = new RenderLayer::CEDXDomainShader(Device, shaderCode);
	if (!shader->Create()) {
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CEGeometryShader* CEDXManager::CreateGeometryShader(
	const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXGeometryShader> shader = new RenderLayer::CEDXGeometryShader(
		Device, shaderCode);
	if (!shader->Create()) {
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CEMeshShader* CEDXManager::CreateMeshShader(const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXMeshShader> shader = new RenderLayer::CEDXMeshShader(Device, shaderCode);
	if (!shader->Create()) {
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CEAmplificationShader* CEDXManager::CreateAmplificationShader(
	const CEArray<uint8>& shaderCode) {
	CERef shader = new RenderLayer::CEDXAmplificationShader(
		Device, shaderCode);
	if (!shader->Create()) {
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CEPixelShader* CEDXManager::CreatePixelShader(const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXPixelShader> shader = new RenderLayer::CEDXPixelShader(Device, shaderCode);
	if (!RenderLayer::CEDXBaseShader::GetShaderReflection(shader.Get())) {
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CERayGenShader* CEDXManager::CreateRayGenShader(const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXRayGenShader> shader = new RenderLayer::CEDXRayGenShader(Device, shaderCode);
	if (!RenderLayer::CEDXBaseRayTracingShader::GetRayTracingShaderReflection(shader.Get())) {
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CERayAnyHitShader* CEDXManager::CreateRayAnyHitShader(
	const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXRayAnyHitShader> shader = new RenderLayer::CEDXRayAnyHitShader(
		Device, shaderCode);
	if (!RenderLayer::CEDXBaseRayTracingShader::GetRayTracingShaderReflection(shader.Get())) {
		CE_LOG_ERROR("[CEDXManager]: Failed to retrive Shader Identifier");
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CERayClosestHitShader* CEDXManager::CreateRayClosestHitShader(
	const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXRayClosestHitShader> shader = new RenderLayer::CEDXRayClosestHitShader(
		Device, shaderCode);
	if (!RenderLayer::CEDXBaseRayTracingShader::GetRayTracingShaderReflection(shader.Get())) {
		CE_LOG_ERROR("[CEDXManager]: Failed to retrive Shader Identifier");
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CERayMissShader*
CEDXManager::CreateRayMissShader(const CEArray<uint8>& shaderCode) {
	CERef<RenderLayer::CEDXRayMissShader> shader = new RenderLayer::CEDXRayMissShader(Device, shaderCode);
	if (!RenderLayer::CEDXBaseRayTracingShader::GetRayTracingShaderReflection(shader.Get())) {
		CE_LOG_ERROR("[CEDXManager]: Failed to retrive Shader Identifier");
		return nullptr;
	}

	return shader.ReleaseOwnership();
}

Main::RenderLayer::CEDepthStencilState* CEDXManager::CreateDepthStencilState(
	const RenderLayer::CEDepthStencilStateCreateInfo& createInfo) {
	D3D12_DEPTH_STENCIL_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	desc.DepthEnable = createInfo.DepthEnable;
	desc.DepthFunc = RenderLayer::ConvertComparisonFunc(createInfo.DepthFunc);
	desc.DepthWriteMask = RenderLayer::ConvertDepthWriteMask(createInfo.DepthWriteMask);
	desc.StencilEnable = createInfo.StencilEnable;
	desc.StencilReadMask = createInfo.StencilReadMask;
	desc.StencilWriteMask = createInfo.StencilWriteMask;
	desc.FrontFace = RenderLayer::ConvertDepthStencilOp(createInfo.FrontFace);
	desc.BackFace = RenderLayer::ConvertDepthStencilOp(createInfo.BackFace);

	return new RenderLayer::CEDXDepthStencilState(Device, desc);
}

Main::RenderLayer::CERasterizerState* CEDXManager::CreateRasterizerState(
	const RenderLayer::CERasterizerStateCreateInfo& createInfo) {
	D3D12_RASTERIZER_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	desc.AntialiasedLineEnable = createInfo.AntialiasedLineEnable;
	desc.ConservativeRaster = (createInfo.EnableConservativeRaster)
		                          ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON
		                          : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	desc.CullMode = RenderLayer::ConvertCullMode(createInfo.CullMode);
	desc.DepthBias = createInfo.DepthBias;
	desc.DepthBiasClamp = createInfo.DepthBiasClamp;
	desc.DepthClipEnable = createInfo.DepthClipEnable;
	desc.SlopeScaledDepthBias = createInfo.SlopeScaledDepthBias;
	desc.FillMode = RenderLayer::ConvertFillMode(createInfo.FillMode);
	desc.ForcedSampleCount = createInfo.ForcedSampleCount;
	desc.FrontCounterClockwise = createInfo.FrontCounterClockwise;
	desc.MultisampleEnable = createInfo.MultisampleEnable;

	return new RenderLayer::CEDXRasterizerState(Device, desc);
}

Main::RenderLayer::CEBlendState* CEDXManager::CreateBlendState(const RenderLayer::CEBlendStateCreateInfo& createInfo) {
	D3D12_BLEND_DESC Desc;
	Memory::CEMemory::Memzero(&Desc);

	Desc.AlphaToCoverageEnable = createInfo.AlphaToCoverageEnable;
	Desc.IndependentBlendEnable = createInfo.independentBlendEnable;
	for (uint32 i = 0; i < 8; i++) {
		Desc.RenderTarget[i].BlendEnable = createInfo.RenderTarget[i].BlendEnable;
		Desc.RenderTarget[i].BlendOp = RenderLayer::ConvertBlendOp(createInfo.RenderTarget[i].BlendOp);
		Desc.RenderTarget[i].BlendOpAlpha = RenderLayer::ConvertBlendOp(createInfo.RenderTarget[i].BlendOpAlpha);
		Desc.RenderTarget[i].DestBlend = RenderLayer::ConvertBlend(createInfo.RenderTarget[i].DestinationBlend);
		Desc.RenderTarget[i].DestBlendAlpha = RenderLayer::ConvertBlend(
			createInfo.RenderTarget[i].DestinationBlendAlpha);
		Desc.RenderTarget[i].SrcBlend = RenderLayer::ConvertBlend(createInfo.RenderTarget[i].SourceBlend);
		Desc.RenderTarget[i].SrcBlendAlpha = RenderLayer::ConvertBlend(createInfo.RenderTarget[i].SourceBlendAlpha);
		Desc.RenderTarget[i].LogicOpEnable = createInfo.RenderTarget[i].LogicOpEnable;
		Desc.RenderTarget[i].LogicOp = RenderLayer::ConvertLogicOp(createInfo.RenderTarget[i].LogicOp);
		Desc.RenderTarget[i].RenderTargetWriteMask = RenderLayer::ConvertRenderTargetWriteState(
			createInfo.RenderTarget[i].RenderTargetWriteMask);
	}

	return new RenderLayer::CEDXBlendState(Device, Desc);
}

Main::RenderLayer::CEInputLayoutState* CEDXManager::CreateInputLayout(
	const RenderLayer::CEInputLayoutStateCreateInfo& createInfo) {
	return new RenderLayer::CEDXInputLayoutState(Device, createInfo);
}

Main::RenderLayer::CEGraphicsPipelineState* CEDXManager::CreateGraphicsPipelineState(
	const RenderLayer::CEGraphicsPipelineStateCreateInfo& createInfo) {
	CERef<RenderLayer::CEDXGraphicsPipelineState> pipelineState = new
		RenderLayer::CEDXGraphicsPipelineState(Device);
	if (!pipelineState->Create(createInfo)) {
		CE_LOG_ERROR("[CEDXManager]: Failed to create GraphicsPipelineState");
		return nullptr;
	}

	return pipelineState.ReleaseOwnership();
}

CEComputePipelineState* CEDXManager::CreateComputePipelineState(const CEComputePipelineStateCreateInfo& createInfo) {
	Assert(createInfo.Shader != nullptr);

	CERef<RenderLayer::CEDXComputeShader> shader = MakeSharedRef<
		RenderLayer::CEDXComputeShader>(createInfo.Shader);
	CERef pipelineState = new RenderLayer::CEDXComputePipelineState(Device, shader);
	if (!pipelineState->Create()) {
		CE_LOG_ERROR("[CEDXManager]: Failed to create ComputePipelineState");
		return nullptr;
	}

	return pipelineState.ReleaseOwnership();
}

CERayTracingPipelineState* CEDXManager::CreateRayTracingPipelineState(
	const CERayTracingPipelineStateCreateInfo& createInfo) {
	CERef<RenderLayer::CEDXRayTracingPipelineState> pipelineState = new
		RenderLayer::CEDXRayTracingPipelineState(Device);
	if (!pipelineState->Create(createInfo)) {
		CE_LOG_ERROR("[CEDXManager]: Failed to create RayTracingPipelineState");
		return nullptr;
	}

	return pipelineState.ReleaseOwnership();
}

Main::RenderLayer::CEGPUProfiler* CEDXManager::CreateProfiler() {
	return RenderLayer::CEDXGPUProfiler::Create(Device);
}

//TODO: check what happened if create viewport from editor!!!!
Main::RenderLayer::CEViewport* CEDXManager::CreateViewport(Core::Platform::Generic::Window::CEWindow* window,
                                                           uint32 width, uint32 height,
                                                           RenderLayer::CEFormat colorFormat,
                                                           RenderLayer::CEFormat depthFormat) {
	CERef<Core::Platform::Windows::Window::CEWindowsWindow> wWindow = MakeSharedRef<
		Core::Platform::Windows::Window::CEWindowsWindow>(window);
	if (width == 0) {
		width = wWindow->GetWidth();
	}

	if (height == 0) {
		height = wWindow->GetHeight();
	}

	CERef<RenderLayer::CEDXViewport> viewport = new RenderLayer::CEDXViewport(
		Device, DirectCommandContext.Get(), wWindow->GetHandle(), colorFormat, width, height);

	if (viewport->Create()) {
		return viewport.ReleaseOwnership();
	}

	return nullptr;
}

Main::RenderLayer::CEICommandContext* CEDXManager::GetDefaultCommandContext() {
	return DirectCommandContext.Get();
}

bool CEDXManager::UAVSupportsFormat(RenderLayer::CEFormat format) {
	D3D12_FEATURE_DATA_D3D12_OPTIONS featureData;
	Memory::CEMemory::Memzero(&featureData, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS));

	HRESULT hResult = Device->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &featureData,
	                                                           sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS));
	if (SUCCEEDED(hResult)) {
		if (featureData.TypedUAVLoadAdditionalFormats) {
			D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = {
				RenderLayer::ConvertFormat(format), D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
			};

			//TODO: create functions inside CEDevice class to prevent calling GetDevice every time.
			hResult = Device->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport,
			                                                   sizeof(formatSupport));
			if (FAILED(hResult) || (formatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) == 0) {
				return false;
			}
		}
	}
	return true;
}

void CEDXManager::CheckRayTracingSupport(Main::CERayTracingSupport& outSupport) {
	D3D12_RAYTRACING_TIER tier = Device->GetRayTracingTier();
	if (tier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
		if (tier == D3D12_RAYTRACING_TIER_1_1) {
			outSupport.Tier = Main::CERayTracingTier::Tier1_1;
		}
		else if (tier == D3D12_RAYTRACING_TIER_1_0) {
			outSupport.Tier = Main::CERayTracingTier::Tier1;
		}

		outSupport.MaxRecursionDepth = D3D12_RAYTRACING_MAX_DECLARABLE_TRACE_RECURSION_DEPTH;
	}
	else {
		outSupport.Tier = Main::CERayTracingTier::NotSupported;
	}
}

void CEDXManager::CheckShadingRateSupport(Main::CEShadingRateSupport& outSupport) {
	D3D12_VARIABLE_SHADING_RATE_TIER tier = Device->GetVariableRateShadingTier();
	if (tier == D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED) {
		outSupport.Tier = Main::CEShadingRateTier::NotSupported;
	}
	else if (tier == D3D12_VARIABLE_SHADING_RATE_TIER_1) {
		outSupport.Tier = Main::CEShadingRateTier::Tier1;
	}
	else if (tier == D3D12_VARIABLE_SHADING_RATE_TIER_2) {
		outSupport.Tier = Main::CEShadingRateTier::Tier2;
	}

	outSupport.ShadingRateImageTileSize = Device->GetVariableRateShadingTileSize();
}


template <typename TCEDXTexture>
TCEDXTexture* CEDXManager::CreateTexture(RenderLayer::CEFormat format, uint32 sizeX, uint32 sizeY, uint32 sizeZ,
                                         uint32 numMips, uint32 numSamplers, uint32 flags,
                                         RenderLayer::CEResourceState initialState,
                                         const RenderLayer::CEResourceData* initialData,
                                         const RenderLayer::CEClearValue& optimalClearValue) {
	CERef<TCEDXTexture> newTexture = new TCEDXTexture(Device,
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
			clearValuePtr = &clearValue;
		}
		else if (optimalClearValue.GetType() == RenderLayer::CEClearValue::CEType::Color) {
			Memory::CEMemory::Memcpy(clearValue.Color, optimalClearValue.AsColor().Elements);
			clearValuePtr = &clearValue;
		}
	}

	CERef<RenderLayer::CEDXResource> resource = new RenderLayer::CEDXResource(
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

		CERef<RenderLayer::CEDXShaderResourceView> SRV = new RenderLayer::CEDXShaderResourceView(
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

		CERef<RenderLayer::CEDXRenderTargetView> RTV = new RenderLayer::CEDXRenderTargetView(
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

		CERef<RenderLayer::CEDXDepthStencilView> DSV = new RenderLayer::CEDXDepthStencilView(
			Device, DepthStencilOfflineDescriptorHeap);

		if (!DSV->Create()) {
			return nullptr;
		}

		if (!DSV->CreateView(newTexture->GetResource(), viewDesc)) {
			return nullptr;
		}

		newTexture2D->SetDepthStencilView(DSV.ReleaseOwnership());
	}

	if (flags & RenderLayer::TextureFlag_UAV && !(flags & TextureFlag_NoDefaultUAV) && isTexture2D) {
		RenderLayer::CEDXTexture2D* newTexture2D = static_cast<RenderLayer::CEDXTexture2D*>(newTexture->AsTexture2D());

		D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc;
		Memory::CEMemory::Memzero(&viewDesc);

		viewDesc.Format = Desc.Format;
		viewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;
		viewDesc.Texture2D.PlaneSlice = 0;

		CERef<RenderLayer::CEDXUnorderedAccessView> UAV = new RenderLayer::CEDXUnorderedAccessView(
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
		CETexture2D* texture2D = newTexture->AsTexture2D();
		if (!texture2D) {
			return nullptr;
		}

		// DirectCommandContext->Execute(
		// [&texture2D, &sizeX, &sizeY, &sizeZ, &initialData, &initialState, this] {
		DirectCommandContext->Begin();
		DirectCommandContext->TransitionTexture(texture2D, CEResourceState::Common, CEResourceState::CopyDest);
		DirectCommandContext->UpdateTexture2D(texture2D, sizeX, sizeY, 0, initialData->GetData());

		DirectCommandContext->TransitionTexture(texture2D, CEResourceState::CopyDest, initialState);
		DirectCommandContext->End();
		// });
	}
	else {
		if (initialState != CEResourceState::Common) {

			// DirectCommandContext->Execute(
			// 	[&newTexture, &initialState, this] {
			DirectCommandContext->Begin();
			DirectCommandContext->TransitionTexture(newTexture.Get(), CEResourceState::Common, initialState);
			DirectCommandContext->End();
			// });
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

	CERef<RenderLayer::CEDXResource> resource = new RenderLayer::CEDXResource(Device, desc, dxHeapType);
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
