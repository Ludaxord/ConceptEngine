#pragma once
#include "../Core/Application/Generic/CEWindow.h"

#include "RenderingCore.h"
#include "Resources.h"
#include "ResourceViews.h"
#include "CommandList.h"
#include "../Managers/CEManager.h"

struct ResourceData;
struct ClearValue;
class RayTracingGeometry;
class RayTracingScene;

enum class ERenderLayerApi : uint32 {
	Unknown = 0,
	D3D12 = 1,
};

inline const char* ToString(ERenderLayerApi RenderLayerApi) {
	switch (RenderLayerApi) {
	case ERenderLayerApi::D3D12: return "D3D12";
	default: return "Unknown";
	}
}

enum class EShadingRateTier {
	NotSupported = 0,
	Tier1 = 1,
	Tier2 = 2,
};

struct ShadingRateSupport {
	EShadingRateTier Tier = EShadingRateTier::NotSupported;
	uint32 ShadingRateImageTileSize = 0;
};

enum class ERayTracingTier {
	NotSupported = 0,
	Tier1 = 1,
	Tier1_1 = 2,
};

struct RayTracingSupport {
	ERayTracingTier Tier;
	uint32 MaxRecursionDepth;
};

class CEGraphicsManager : public CEManager {
public:
	CEGraphicsManager(ERenderLayerApi InApi)
		: Api(InApi) {
	}

	virtual ~CEGraphicsManager() = default;

	virtual bool Create(bool EnableDebug) = 0;

	virtual Texture2D* CreateTexture2D(
		EFormat Format,
		uint32 Width,
		uint32 Height,
		uint32 NumMips,
		uint32 NumSamples,
		uint32 Flags,
		EResourceState InitialState,
		const ResourceData* InitalData,
		const ClearValue& OptimizedClearValue) = 0;

	virtual Texture2DArray* CreateTexture2DArray(
		EFormat Format,
		uint32 Width,
		uint32 Height,
		uint32 NumMips,
		uint32 NumSamples,
		uint32 NumArraySlices,
		uint32 Flags,
		EResourceState InitialState,
		const ResourceData* InitalData,
		const ClearValue& OptimizedClearValue) = 0;

	virtual TextureCube* CreateTextureCube(
		EFormat Format,
		uint32 Size,
		uint32 NumMips,
		uint32 Flags,
		EResourceState InitialState,
		const ResourceData* InitalData,
		const ClearValue& OptimizedClearValue) = 0;

	virtual TextureCubeArray* CreateTextureCubeArray(
		EFormat Format,
		uint32 Size,
		uint32 NumMips,
		uint32 NumArraySlices,
		uint32 Flags,
		EResourceState InitialState,
		const ResourceData* InitalData,
		const ClearValue& OptimizedClearValue) = 0;

	virtual Texture3D* CreateTexture3D(
		EFormat Format,
		uint32 Width,
		uint32 Height,
		uint32 Depth,
		uint32 NumMips,
		uint32 Flags,
		EResourceState InitialState,
		const ResourceData* InitalData,
		const ClearValue& OptimizedClearValue) = 0;

	virtual class SamplerState* CreateSamplerState(const struct SamplerStateCreateInfo& CreateInfo) = 0;

	virtual VertexBuffer* CreateVertexBuffer(uint32 Stride, uint32 NumVertices, uint32 Flags,
	                                         EResourceState InitialState, const ResourceData* InitalData) = 0;
	virtual IndexBuffer* CreateIndexBuffer(EIndexFormat Format, uint32 NumIndices, uint32 Flags,
	                                       EResourceState InitialState, const ResourceData* InitalData) = 0;
	virtual ConstantBuffer* CreateConstantBuffer(uint32 Size, uint32 Flags, EResourceState InitialState,
	                                             const ResourceData* InitalData) = 0;
	virtual StructuredBuffer* CreateStructuredBuffer(uint32 Stride, uint32 NumElements, uint32 Flags,
	                                                 EResourceState InitialState, const ResourceData* InitalData) = 0;

	virtual RayTracingScene* CreateRayTracingScene(uint32 Flags, RayTracingGeometryInstance* Instances,
	                                               uint32 NumInstances) = 0;
	virtual RayTracingGeometry* CreateRayTracingGeometry(uint32 Flags, VertexBuffer* VertexBuffer,
	                                                     IndexBuffer* IndexBuffer) = 0;

	virtual ShaderResourceView* CreateShaderResourceView(const ShaderResourceViewCreateInfo& CreateInfo) = 0;
	virtual UnorderedAccessView* CreateUnorderedAccessView(const UnorderedAccessViewCreateInfo& CreateInfo) = 0;
	virtual RenderTargetView* CreateRenderTargetView(const RenderTargetViewCreateInfo& CreateInfo) = 0;
	virtual DepthStencilView* CreateDepthStencilView(const DepthStencilViewCreateInfo& CreateInfo) = 0;

	virtual class ComputeShader* CreateComputeShader(const CEArray<uint8>& ShaderCode) = 0;

	virtual class VertexShader* CreateVertexShader(const CEArray<uint8>& ShaderCode) = 0;
	virtual class HullShader* CreateHullShader(const CEArray<uint8>& ShaderCode) = 0;
	virtual class DomainShader* CreateDomainShader(const CEArray<uint8>& ShaderCode) = 0;
	virtual class GeometryShader* CreateGeometryShader(const CEArray<uint8>& ShaderCode) = 0;
	virtual class MeshShader* CreateMeshShader(const CEArray<uint8>& ShaderCode) = 0;
	virtual class AmplificationShader* CreateAmplificationShader(const CEArray<uint8>& ShaderCode) = 0;
	virtual class PixelShader* CreatePixelShader(const CEArray<uint8>& ShaderCode) = 0;

	virtual class RayGenShader* CreateRayGenShader(const CEArray<uint8>& ShaderCode) = 0;
	virtual class RayAnyHitShader* CreateRayAnyHitShader(const CEArray<uint8>& ShaderCode) = 0;
	virtual class RayClosestHitShader* CreateRayClosestHitShader(const CEArray<uint8>& ShaderCode) = 0;
	virtual class RayMissShader* CreateRayMissShader(const CEArray<uint8>& ShaderCode) = 0;

	virtual class DepthStencilState* CreateDepthStencilState(const DepthStencilStateCreateInfo& CreateInfo) = 0;

	virtual class RasterizerState* CreateRasterizerState(const RasterizerStateCreateInfo& CreateInfo) = 0;

	virtual class BlendState* CreateBlendState(const BlendStateCreateInfo& CreateInfo) = 0;

	virtual class InputLayoutState* CreateInputLayout(const InputLayoutStateCreateInfo& CreateInfo) = 0;

	virtual class GraphicsPipelineState* CreateGraphicsPipelineState(const GraphicsPipelineStateCreateInfo& CreateInfo)
	= 0;
	virtual class ComputePipelineState* CreateComputePipelineState(const ComputePipelineStateCreateInfo& CreateInfo) =
	0;
	virtual class RayTracingPipelineState* CreateRayTracingPipelineState(
		const RayTracingPipelineStateCreateInfo& CreateInfo) = 0;

	virtual class GPUProfiler* CreateProfiler() = 0;

	virtual class Viewport* CreateViewport(CEWindow* Window, uint32 Width, uint32 Height, EFormat ColorFormat,
	                                       EFormat DepthFormat) = 0;

	virtual class ICommandContext* GetDefaultCommandContext() = 0;

	virtual std::string GetAdapterName() {
		return std::string();
	}

	virtual void CheckRayTracingSupport(RayTracingSupport& OutSupport) = 0;
	virtual void CheckShadingRateSupport(ShadingRateSupport& OutSupport) = 0;

	virtual bool UAVSupportsFormat(EFormat Format) {
		UNREFERENCED_VARIABLE(Format);
		return false;
	}

	ERenderLayerApi GetApi() {
		return Api;
	}

	bool Create() override {
		const bool EnableDebug =
#if ENABLE_API_DEBUGGING
        true;
#else
			false;
#endif
		return Create(EnableDebug);
	};

	void Release() override {

	};
private:
	ERenderLayerApi Api;
};

FORCEINLINE Texture2D* CreateTexture2D(
	EFormat Format,
	uint32 Width,
	uint32 Height,
	uint32 NumMips,
	uint32 NumSamples,
	uint32 Flags,
	EResourceState InitialState,
	const ResourceData* InitialData,
	const ClearValue& OptimizedClearValue = ClearValue()) {
	return GraphicsManager->CreateTexture2D(Format, Width, Height, NumMips, NumSamples, Flags, InitialState,
	                                              InitialData,
	                                              OptimizedClearValue);
}

FORCEINLINE Texture2DArray* CreateTexture2DArray(
	EFormat Format,
	uint32 Width,
	uint32 Height,
	uint32 NumMips,
	uint32 NumSamples,
	uint32 NumArraySlices,
	uint32 Flags,
	EResourceState InitialState,
	const ResourceData* InitialData,
	const ClearValue& OptimizedClearValue = ClearValue()) {
	return GraphicsManager->CreateTexture2DArray(Format, Width, Height, NumMips, NumSamples, NumArraySlices,
	                                                   Flags,
	                                                   InitialState, InitialData, OptimizedClearValue);
}

FORCEINLINE TextureCube* CreateTextureCube(
	EFormat Format,
	uint32 Size,
	uint32 NumMips,
	uint32 Flags,
	EResourceState InitialState,
	const ResourceData* InitialData,
	const ClearValue& OptimizedClearValue = ClearValue()) {
	return GraphicsManager->CreateTextureCube(Format, Size, NumMips, Flags, InitialState, InitialData,
	                                                OptimizedClearValue);
}

FORCEINLINE TextureCubeArray* CreateTextureCubeArray(
	EFormat Format,
	uint32 Size,
	uint32 NumMips,
	uint32 NumArraySlices,
	uint32 Flags,
	EResourceState InitialState,
	const ResourceData* InitialData,
	const ClearValue& OptimizedClearValue = ClearValue()) {
	return GraphicsManager->CreateTextureCubeArray(Format, Size, NumMips, NumArraySlices, Flags, InitialState,
	                                                     InitialData,
	                                                     OptimizedClearValue);
}

FORCEINLINE Texture3D* CreateTexture3D(
	EFormat Format,
	uint32 Width,
	uint32 Height,
	uint32 Depth,
	uint32 NumMips,
	uint32 Flags,
	EResourceState InitialState,
	const ResourceData* InitialData,
	const ClearValue& OptimizedClearValue = ClearValue()) {
	return GraphicsManager->CreateTexture3D(Format, Width, Height, Depth, NumMips, Flags, InitialState,
	                                              InitialData,
	                                              OptimizedClearValue);
}

FORCEINLINE class SamplerState* CreateSamplerState(const struct SamplerStateCreateInfo& CreateInfo) {
	return GraphicsManager->CreateSamplerState(CreateInfo);
}

FORCEINLINE VertexBuffer* CreateVertexBuffer(
	uint32 Stride,
	uint32 NumVertices,
	uint32 Flags,
	EResourceState InitialState,
	const ResourceData* InitialData) {
	return GraphicsManager->CreateVertexBuffer(Stride, NumVertices, Flags, InitialState, InitialData);
}

template <typename T>
FORCEINLINE VertexBuffer* CreateVertexBuffer(uint32 NumVertices, uint32 Flags, EResourceState InitialState,
                                             const ResourceData* InitialData) {
	constexpr uint32 STRIDE = sizeof(T);
	return CreateVertexBuffer(STRIDE, NumVertices, Flags, InitialState, InitialData);
}

FORCEINLINE IndexBuffer* CreateIndexBuffer(
	EIndexFormat Format,
	uint32 NumIndices,
	uint32 Flags,
	EResourceState InitialState,
	const ResourceData* InitialData) {
	return GraphicsManager->CreateIndexBuffer(Format, NumIndices, Flags, InitialState, InitialData);
}

FORCEINLINE ConstantBuffer* CreateConstantBuffer(uint32 Size, uint32 Flags, EResourceState InitialState,
                                                 const ResourceData* InitialData) {
	return GraphicsManager->CreateConstantBuffer(Size, Flags, InitialState, InitialData);
}

template <typename T>
FORCEINLINE ConstantBuffer* CreateConstantBuffer(uint32 Flags, EResourceState InitialState,
                                                 const ResourceData* InitialData) {
	constexpr uint32 SIZE_IN_BYTES = sizeof(T);
	return CreateConstantBuffer(SIZE_IN_BYTES, Flags, InitialState, InitialData);
}

FORCEINLINE StructuredBuffer* CreateStructuredBuffer(
	uint32 Stride,
	uint32 NumElements,
	uint32 Flags,
	EResourceState InitialState,
	const ResourceData* InitialData) {
	return GraphicsManager->CreateStructuredBuffer(Stride, NumElements, Flags, InitialState, InitialData);
}

FORCEINLINE RayTracingScene* CreateRayTracingScene(uint32 Flags, RayTracingGeometryInstance* Instances,
                                                   uint32 NumInstances) {
	return GraphicsManager->CreateRayTracingScene(Flags, Instances, NumInstances);
}

FORCEINLINE RayTracingGeometry* CreateRayTracingGeometry(uint32 Flags, VertexBuffer* VertexBuffer,
                                                         IndexBuffer* IndexBuffer) {
	return GraphicsManager->CreateRayTracingGeometry(Flags, VertexBuffer, IndexBuffer);
}

FORCEINLINE ShaderResourceView* CreateShaderResourceView(const ShaderResourceViewCreateInfo& CreateInfo) {
	return GraphicsManager->CreateShaderResourceView(CreateInfo);
}

FORCEINLINE ShaderResourceView* CreateShaderResourceView(Texture2D* Texture, EFormat Format, uint32 Mip, uint32 NumMips,
                                                         float MinMipBias) {
	ShaderResourceViewCreateInfo CreateInfo(ShaderResourceViewCreateInfo::EType::Texture2D);
	CreateInfo.Texture2D.Texture = Texture;
	CreateInfo.Texture2D.Format = Format;
	CreateInfo.Texture2D.Mip = Mip;
	CreateInfo.Texture2D.NumMips = NumMips;
	CreateInfo.Texture2D.MinMipBias = MinMipBias;
	return CreateShaderResourceView(CreateInfo);
}

FORCEINLINE ShaderResourceView* CreateShaderResourceView(
	Texture2DArray* Texture,
	EFormat Format,
	uint32 Mip,
	uint32 NumMips,
	uint32 ArraySlice,
	uint32 NumArraySlices,
	float MinMipBias) {
	ShaderResourceViewCreateInfo CreateInfo(ShaderResourceViewCreateInfo::EType::Texture2DArray);
	CreateInfo.Texture2DArray.Texture = Texture;
	CreateInfo.Texture2DArray.Format = Format;
	CreateInfo.Texture2DArray.Mip = Mip;
	CreateInfo.Texture2DArray.NumMips = NumMips;
	CreateInfo.Texture2DArray.ArraySlice = ArraySlice;
	CreateInfo.Texture2DArray.NumArraySlices = NumArraySlices;
	CreateInfo.Texture2DArray.MinMipBias = MinMipBias;
	return CreateShaderResourceView(CreateInfo);
}

FORCEINLINE ShaderResourceView* CreateShaderResourceView(TextureCube* Texture, EFormat Format, uint32 Mip,
                                                         uint32 NumMips, float MinMipBias) {
	ShaderResourceViewCreateInfo CreateInfo(ShaderResourceViewCreateInfo::EType::TextureCube);
	CreateInfo.TextureCube.Texture = Texture;
	CreateInfo.TextureCube.Format = Format;
	CreateInfo.TextureCube.Mip = Mip;
	CreateInfo.TextureCube.NumMips = NumMips;
	CreateInfo.TextureCube.MinMipBias = MinMipBias;
	return CreateShaderResourceView(CreateInfo);
}

FORCEINLINE ShaderResourceView* CreateShaderResourceView(
	TextureCubeArray* Texture,
	EFormat Format,
	uint32 Mip,
	uint32 NumMips,
	uint32 ArraySlice,
	uint32 NumArraySlices,
	float MinMipBias) {
	ShaderResourceViewCreateInfo CreateInfo(ShaderResourceViewCreateInfo::EType::TextureCubeArray);
	CreateInfo.TextureCubeArray.Texture = Texture;
	CreateInfo.TextureCubeArray.Format = Format;
	CreateInfo.TextureCubeArray.Mip = Mip;
	CreateInfo.TextureCubeArray.NumMips = NumMips;
	CreateInfo.TextureCubeArray.ArraySlice = ArraySlice;
	CreateInfo.TextureCubeArray.NumArraySlices = NumArraySlices;
	CreateInfo.TextureCubeArray.MinMipBias = MinMipBias;
	return CreateShaderResourceView(CreateInfo);
}

FORCEINLINE ShaderResourceView* CreateShaderResourceView(
	Texture3D* Texture,
	EFormat Format,
	uint32 Mip,
	uint32 NumMips,
	uint32 DepthSlice,
	uint32 NumDepthSlices,
	float MinMipBias) {
	ShaderResourceViewCreateInfo CreateInfo(ShaderResourceViewCreateInfo::EType::Texture3D);
	CreateInfo.Texture3D.Texture = Texture;
	CreateInfo.Texture3D.Format = Format;
	CreateInfo.Texture3D.Mip = Mip;
	CreateInfo.Texture3D.NumMips = NumMips;
	CreateInfo.Texture3D.DepthSlice = DepthSlice;
	CreateInfo.Texture3D.NumDepthSlices = NumDepthSlices;
	CreateInfo.Texture3D.MinMipBias = MinMipBias;
	return CreateShaderResourceView(CreateInfo);
}

FORCEINLINE ShaderResourceView* CreateShaderResourceView(VertexBuffer* Buffer, uint32 FirstVertex, uint32 NumVertices) {
	ShaderResourceViewCreateInfo CreateInfo(ShaderResourceViewCreateInfo::EType::VertexBuffer);
	CreateInfo.VertexBuffer.Buffer = Buffer;
	CreateInfo.VertexBuffer.FirstVertex = FirstVertex;
	CreateInfo.VertexBuffer.NumVertices = NumVertices;
	return CreateShaderResourceView(CreateInfo);
}

FORCEINLINE ShaderResourceView* CreateShaderResourceView(IndexBuffer* Buffer, uint32 FirstIndex, uint32 NumIndices) {
	ShaderResourceViewCreateInfo CreateInfo(ShaderResourceViewCreateInfo::EType::IndexBuffer);
	CreateInfo.IndexBuffer.Buffer = Buffer;
	CreateInfo.IndexBuffer.FirstIndex = FirstIndex;
	CreateInfo.IndexBuffer.NumIndices = NumIndices;
	return CreateShaderResourceView(CreateInfo);
}

FORCEINLINE ShaderResourceView* CreateShaderResourceView(StructuredBuffer* Buffer, uint32 FirstElement,
                                                         uint32 NumElements) {
	ShaderResourceViewCreateInfo CreateInfo(ShaderResourceViewCreateInfo::EType::StructuredBuffer);
	CreateInfo.StructuredBuffer.Buffer = Buffer;
	CreateInfo.StructuredBuffer.FirstElement = FirstElement;
	CreateInfo.StructuredBuffer.NumElements = NumElements;
	return CreateShaderResourceView(CreateInfo);
}

FORCEINLINE UnorderedAccessView* CreateUnorderedAccessView(const UnorderedAccessViewCreateInfo& CreateInfo) {
	return GraphicsManager->CreateUnorderedAccessView(CreateInfo);
}

FORCEINLINE UnorderedAccessView* CreateUnorderedAccessView(Texture2D* Texture, EFormat Format, uint32 Mip) {
	UnorderedAccessViewCreateInfo CreateInfo(UnorderedAccessViewCreateInfo::EType::Texture2D);
	CreateInfo.Texture2D.Texture = Texture;
	CreateInfo.Texture2D.Format = Format;
	CreateInfo.Texture2D.Mip = Mip;
	return CreateUnorderedAccessView(CreateInfo);
}

FORCEINLINE UnorderedAccessView* CreateUnorderedAccessView(Texture2DArray* Texture, EFormat Format, uint32 Mip,
                                                           uint32 ArraySlice, uint32 NumArraySlices) {
	UnorderedAccessViewCreateInfo CreateInfo(UnorderedAccessViewCreateInfo::EType::Texture2DArray);
	CreateInfo.Texture2DArray.Texture = Texture;
	CreateInfo.Texture2DArray.Format = Format;
	CreateInfo.Texture2DArray.Mip = Mip;
	CreateInfo.Texture2DArray.ArraySlice = ArraySlice;
	CreateInfo.Texture2DArray.NumArraySlices = NumArraySlices;
	return CreateUnorderedAccessView(CreateInfo);
}

FORCEINLINE UnorderedAccessView* CreateUnorderedAccessView(TextureCube* Texture, EFormat Format, uint32 Mip) {
	UnorderedAccessViewCreateInfo CreateInfo(UnorderedAccessViewCreateInfo::EType::TextureCube);
	CreateInfo.TextureCube.Texture = Texture;
	CreateInfo.TextureCube.Format = Format;
	CreateInfo.TextureCube.Mip = Mip;
	return CreateUnorderedAccessView(CreateInfo);
}

FORCEINLINE UnorderedAccessView* CreateUnorderedAccessView(TextureCubeArray* Texture, EFormat Format, uint32 Mip,
                                                           uint32 ArraySlice, uint32 NumArraySlices) {
	UnorderedAccessViewCreateInfo CreateInfo(UnorderedAccessViewCreateInfo::EType::TextureCubeArray);
	CreateInfo.TextureCubeArray.Texture = Texture;
	CreateInfo.TextureCubeArray.Format = Format;
	CreateInfo.TextureCubeArray.Mip = Mip;
	CreateInfo.TextureCubeArray.ArraySlice = ArraySlice;
	CreateInfo.TextureCubeArray.NumArraySlices = NumArraySlices;
	return CreateUnorderedAccessView(CreateInfo);
}

FORCEINLINE UnorderedAccessView* CreateUnorderedAccessView(Texture3D* Texture, EFormat Format, uint32 Mip,
                                                           uint32 DepthSlice, uint32 NumDepthSlices) {
	UnorderedAccessViewCreateInfo CreateInfo(UnorderedAccessViewCreateInfo::EType::Texture3D);
	CreateInfo.Texture3D.Texture = Texture;
	CreateInfo.Texture3D.Format = Format;
	CreateInfo.Texture3D.Mip = Mip;
	CreateInfo.Texture3D.DepthSlice = DepthSlice;
	CreateInfo.Texture3D.NumDepthSlices = NumDepthSlices;
	return CreateUnorderedAccessView(CreateInfo);
}

FORCEINLINE UnorderedAccessView*
CreateUnorderedAccessView(VertexBuffer* Buffer, uint32 FirstVertex, uint32 NumVertices) {
	UnorderedAccessViewCreateInfo CreateInfo(UnorderedAccessViewCreateInfo::EType::VertexBuffer);
	CreateInfo.VertexBuffer.Buffer = Buffer;
	CreateInfo.VertexBuffer.FirstVertex = FirstVertex;
	CreateInfo.VertexBuffer.NumVertices = NumVertices;
	return CreateUnorderedAccessView(CreateInfo);
}

FORCEINLINE UnorderedAccessView* CreateUnorderedAccessView(IndexBuffer* Buffer, uint32 FirstIndex, uint32 NumIndices) {
	UnorderedAccessViewCreateInfo CreateInfo(UnorderedAccessViewCreateInfo::EType::IndexBuffer);
	CreateInfo.IndexBuffer.Buffer = Buffer;
	CreateInfo.IndexBuffer.FirstIndex = FirstIndex;
	CreateInfo.IndexBuffer.NumIndices = NumIndices;
	return CreateUnorderedAccessView(CreateInfo);
}

FORCEINLINE UnorderedAccessView* CreateUnorderedAccessView(StructuredBuffer* Buffer, uint32 FirstElement,
                                                           uint32 NumElements) {
	UnorderedAccessViewCreateInfo CreateInfo(UnorderedAccessViewCreateInfo::EType::StructuredBuffer);
	CreateInfo.StructuredBuffer.Buffer = Buffer;
	CreateInfo.StructuredBuffer.FirstElement = FirstElement;
	CreateInfo.StructuredBuffer.NumElements = NumElements;
	return CreateUnorderedAccessView(CreateInfo);
}

FORCEINLINE RenderTargetView* CreateRenderTargetView(const RenderTargetViewCreateInfo& CreateInfo) {
	return GraphicsManager->CreateRenderTargetView(CreateInfo);
}

FORCEINLINE RenderTargetView* CreateRenderTargetView(Texture2D* Texture, EFormat Format, uint32 Mip) {
	RenderTargetViewCreateInfo CreateInfo(RenderTargetViewCreateInfo::EType::Texture2D);
	CreateInfo.Format = Format;
	CreateInfo.Texture2D.Texture = Texture;
	CreateInfo.Texture2D.Mip = Mip;
	return CreateRenderTargetView(CreateInfo);
}

FORCEINLINE RenderTargetView* CreateRenderTargetView(Texture2DArray* Texture, EFormat Format, uint32 Mip,
                                                     uint32 ArraySlice, uint32 NumArraySlices) {
	RenderTargetViewCreateInfo CreateInfo(RenderTargetViewCreateInfo::EType::Texture2DArray);
	CreateInfo.Format = Format;
	CreateInfo.Texture2DArray.Texture = Texture;
	CreateInfo.Texture2DArray.Mip = Mip;
	CreateInfo.Texture2DArray.ArraySlice = ArraySlice;
	CreateInfo.Texture2DArray.NumArraySlices = NumArraySlices;
	return CreateRenderTargetView(CreateInfo);
}

FORCEINLINE RenderTargetView* CreateRenderTargetView(TextureCube* Texture, EFormat Format, ECubeFace CubeFace,
                                                     uint32 Mip) {
	RenderTargetViewCreateInfo CreateInfo(RenderTargetViewCreateInfo::EType::TextureCube);
	CreateInfo.Format = Format;
	CreateInfo.TextureCube.Texture = Texture;
	CreateInfo.TextureCube.Mip = Mip;
	CreateInfo.TextureCube.CubeFace = CubeFace;
	return CreateRenderTargetView(CreateInfo);
}

FORCEINLINE RenderTargetView* CreateRenderTargetView(TextureCubeArray* Texture, EFormat Format, ECubeFace CubeFace,
                                                     uint32 Mip, uint32 ArraySlice) {
	RenderTargetViewCreateInfo CreateInfo(RenderTargetViewCreateInfo::EType::TextureCubeArray);
	CreateInfo.Format = Format;
	CreateInfo.TextureCubeArray.Texture = Texture;
	CreateInfo.TextureCubeArray.Mip = Mip;
	CreateInfo.TextureCubeArray.ArraySlice = ArraySlice;
	CreateInfo.TextureCubeArray.CubeFace = CubeFace;
	return CreateRenderTargetView(CreateInfo);
}

FORCEINLINE RenderTargetView* CreateRenderTargetView(Texture3D* Texture, EFormat Format, uint32 Mip, uint32 DepthSlice,
                                                     uint32 NumDepthSlices) {
	RenderTargetViewCreateInfo CreateInfo(RenderTargetViewCreateInfo::EType::Texture3D);
	CreateInfo.Format = Format;
	CreateInfo.Texture3D.Texture = Texture;
	CreateInfo.Texture3D.Mip = Mip;
	CreateInfo.Texture3D.DepthSlice = DepthSlice;
	CreateInfo.Texture3D.NumDepthSlices = NumDepthSlices;
	return CreateRenderTargetView(CreateInfo);
}

FORCEINLINE DepthStencilView* CreateDepthStencilView(const DepthStencilViewCreateInfo& CreateInfo) {
	return GraphicsManager->CreateDepthStencilView(CreateInfo);
}

FORCEINLINE DepthStencilView* CreateDepthStencilView(Texture2D* Texture, EFormat Format, uint32 Mip) {
	DepthStencilViewCreateInfo CreateInfo(DepthStencilViewCreateInfo::EType::Texture2D);
	CreateInfo.Format = Format;
	CreateInfo.Texture2D.Texture = Texture;
	CreateInfo.Texture2D.Mip = Mip;
	return CreateDepthStencilView(CreateInfo);
}

FORCEINLINE DepthStencilView* CreateDepthStencilView(Texture2DArray* Texture, EFormat Format, uint32 Mip,
                                                     uint32 ArraySlice, uint32 NumArraySlices) {
	DepthStencilViewCreateInfo CreateInfo(DepthStencilViewCreateInfo::EType::Texture2DArray);
	CreateInfo.Format = Format;
	CreateInfo.Texture2DArray.Texture = Texture;
	CreateInfo.Texture2DArray.Mip = Mip;
	CreateInfo.Texture2DArray.ArraySlice = ArraySlice;
	CreateInfo.Texture2DArray.NumArraySlices = NumArraySlices;
	return CreateDepthStencilView(CreateInfo);
}

FORCEINLINE DepthStencilView* CreateDepthStencilView(TextureCube* Texture, EFormat Format, ECubeFace CubeFace,
                                                     uint32 Mip) {
	DepthStencilViewCreateInfo CreateInfo(DepthStencilViewCreateInfo::EType::TextureCube);
	CreateInfo.Format = Format;
	CreateInfo.TextureCube.Texture = Texture;
	CreateInfo.TextureCube.Mip = Mip;
	CreateInfo.TextureCube.CubeFace = CubeFace;
	return CreateDepthStencilView(CreateInfo);
}

FORCEINLINE DepthStencilView* CreateDepthStencilView(TextureCubeArray* Texture, EFormat Format, ECubeFace CubeFace,
                                                     uint32 Mip, uint32 ArraySlice) {
	DepthStencilViewCreateInfo CreateInfo(DepthStencilViewCreateInfo::EType::TextureCubeArray);
	CreateInfo.Format = Format;
	CreateInfo.TextureCubeArray.Texture = Texture;
	CreateInfo.TextureCubeArray.Mip = Mip;
	CreateInfo.TextureCubeArray.ArraySlice = ArraySlice;
	CreateInfo.TextureCubeArray.CubeFace = CubeFace;
	return CreateDepthStencilView(CreateInfo);
}

FORCEINLINE ComputeShader* CreateComputeShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateComputeShader(ShaderCode);
}

FORCEINLINE VertexShader* CreateVertexShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateVertexShader(ShaderCode);
}

FORCEINLINE HullShader* CreateHullShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateHullShader(ShaderCode);
}

FORCEINLINE DomainShader* CreateDomainShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateDomainShader(ShaderCode);
}

FORCEINLINE GeometryShader* CreateGeometryShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateGeometryShader(ShaderCode);
}

FORCEINLINE MeshShader* CreateMeshShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateMeshShader(ShaderCode);
}

FORCEINLINE AmplificationShader* CreateAmplificationShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateAmplificationShader(ShaderCode);
}

FORCEINLINE PixelShader* CreatePixelShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreatePixelShader(ShaderCode);
}

FORCEINLINE RayGenShader* CreateRayGenShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateRayGenShader(ShaderCode);
}

FORCEINLINE RayAnyHitShader* CreateRayAnyHitShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateRayAnyHitShader(ShaderCode);
}

FORCEINLINE RayClosestHitShader* CreateRayClosestHitShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateRayClosestHitShader(ShaderCode);
}

FORCEINLINE RayMissShader* CreateRayMissShader(const CEArray<uint8>& ShaderCode) {
	return GraphicsManager->CreateRayMissShader(ShaderCode);
}

FORCEINLINE InputLayoutState* CreateInputLayout(const InputLayoutStateCreateInfo& CreateInfo) {
	return GraphicsManager->CreateInputLayout(CreateInfo);
}

FORCEINLINE DepthStencilState* CreateDepthStencilState(const DepthStencilStateCreateInfo& CreateInfo) {
	return GraphicsManager->CreateDepthStencilState(CreateInfo);
}

FORCEINLINE RasterizerState* CreateRasterizerState(const RasterizerStateCreateInfo& CreateInfo) {
	return GraphicsManager->CreateRasterizerState(CreateInfo);
}

FORCEINLINE BlendState* CreateBlendState(const BlendStateCreateInfo& CreateInfo) {
	return GraphicsManager->CreateBlendState(CreateInfo);
}

FORCEINLINE ComputePipelineState* CreateComputePipelineState(const ComputePipelineStateCreateInfo& CreateInfo) {
	return GraphicsManager->CreateComputePipelineState(CreateInfo);
}

FORCEINLINE GraphicsPipelineState* CreateGraphicsPipelineState(const GraphicsPipelineStateCreateInfo& CreateInfo) {
	return GraphicsManager->CreateGraphicsPipelineState(CreateInfo);
}

FORCEINLINE RayTracingPipelineState*
CreateRayTracingPipelineState(const RayTracingPipelineStateCreateInfo& CreateInfo) {
	return GraphicsManager->CreateRayTracingPipelineState(CreateInfo);
}

FORCEINLINE class GPUProfiler* CreateProfiler() {
	return GraphicsManager->CreateProfiler();
}

FORCEINLINE class Viewport* CreateViewport(CEWindow* Window, uint32 Width, uint32 Height, EFormat ColorFormat,
                                           EFormat DepthFormat) {
	return GraphicsManager->CreateViewport(Window, Width, Height, ColorFormat, DepthFormat);
}

FORCEINLINE bool UAVSupportsFormat(EFormat Format) {
	return GraphicsManager->UAVSupportsFormat(Format);
}

FORCEINLINE class ICommandContext* GetDefaultCommandContext() {
	return GraphicsManager->GetDefaultCommandContext();
}

FORCEINLINE std::string GetAdapterName() {
	return GraphicsManager->GetAdapterName();
}

FORCEINLINE void CheckShadingRateSupport(ShadingRateSupport& OutSupport) {
	GraphicsManager->CheckShadingRateSupport(OutSupport);
}

FORCEINLINE void CheckRayTracingSupport(RayTracingSupport& OutSupport) {
	GraphicsManager->CheckRayTracingSupport(OutSupport);
}

FORCEINLINE bool IsRayTracingSupported() {
	RayTracingSupport Support;
	CheckRayTracingSupport(Support);

	return Support.Tier != ERayTracingTier::NotSupported;
}

FORCEINLINE bool IsShadingRateSupported() {
	ShadingRateSupport Support;
	CheckShadingRateSupport(Support);

	return Support.Tier != EShadingRateTier::NotSupported;
}
