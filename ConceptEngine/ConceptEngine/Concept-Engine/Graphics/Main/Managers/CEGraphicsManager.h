#pragma once

#include <string>

#include "../../../Core/Common/CEManager.h"
#include "../../../Core/Common/CETypes.h"
#include "../../../Core/Containers/CEArray.h"

#include "../RenderLayer/CERendering.h"
#include "../RenderLayer/CEResourceViews.h"


#define ENABLE_API_DEBUGGING 0;
#define ENABLE_API_GPU_DEBUGGING 0;
#define ENABLE_API_GPU_BREADCRUMBS 0;

namespace ConceptEngine {namespace Core {namespace Platform {namespace Generic {namespace Window {
	class CEWindow;
}}}}}

namespace ConceptEngine::Graphics::Main {namespace RenderLayer {
		class CEICommandContext;
		class CEViewport;
		class CEGPUProfiler;
		struct CERayTracingPipelineStateCreateInfo;
		class CERayTracingPipelineState;
		struct CEComputePipelineStateCreateInfo;
		class CEComputePipelineState;
		struct CEGraphicsPipelineStateCreateInfo;
		class CEGraphicsPipelineState;
		struct CEInputLayoutStateCreateInfo;
		class CEInputLayoutState;
		struct CEBlendStateCreateInfo;
		class CEBlendState;
		struct CERasterizerStateCreateInfo;
		class CERasterizerState;
		struct CEDepthStencilStateCreateInfo;
		class CEDepthStencilState;
		class CERayMissShader;
		class CERayClosestHitShader;
		class CERayAnyHitShader;
		class CERayGenShader;
		class CEPixelShader;
		class CEAmplificationShader;
		class CEMeshShader;
		class CEGeometryShader;
		class CEDomainShader;
		class CEHullShader;
		class CEVertexShader;
		class CEComputeShader;
		class CEShaderResourceView;
		class CERayTracingGeometry;
		struct CERayTracingGeometryInstance;
		class CERayTracingScene;
		class CEStructuredBuffer;
		class CEConstantBuffer;
		enum class CEIndexFormat;
		class CEIndexBuffer;
		class CEVertexBuffer;
		struct CESamplerStateCreateInfo;
		class CESamplerState;
		class CETexture3D;
		class CETextureCubeArray;
		class CETextureCube;
		class CETexture2DArray;
		struct CEResourceData;
		enum class CEResourceState;
		enum class CEFormat;
		class CETexture2D;
	}

	enum class CEShadingRateTier {
		NotSupported = 0,
		Tier1 = 1,
		Tier2 = 2
	};

	struct CEShadingRateSupport {
		CEShadingRateTier Tier = CEShadingRateTier::NotSupported;
		uint32 ShadingRateImageTileSize = 0;
	};

	enum class CERayTracingTier {
		NotSupported = 0,
		Tier1 = 1,
		Tier1_1 = 2
	};

	struct CERayTracingSupport {
		CERayTracingTier Tier;
		uint32 MaxRecursionDepth;
	};

	namespace Managers {

		class CEGraphicsManager : public Core::Common::CEManager {
		public:
			CEGraphicsManager(): CEManager() {
			};
			
			virtual ~CEGraphicsManager() override = default;

			virtual bool Create() override {
				return true;
			};
			
			virtual void Destroy() = 0;

			virtual RenderLayer::CETexture2D* CreateTexture2D(RenderLayer::CEFormat format,
			                                                  uint32 width,
			                                                  uint32 height,
			                                                  uint32 numMips,
			                                                  uint32 numSamples,
			                                                  uint32 flags,
			                                                  RenderLayer::CEResourceState initialState,
			                                                  const RenderLayer::CEResourceData* initialData,
			                                                  const RenderLayer::CEClearValue& optimizedClearValue =
				                                                  RenderLayer::CEClearValue()) = 0;
			virtual RenderLayer::CETexture2DArray* CreateTexture2DArray(RenderLayer::CEFormat format,
			                                                            uint32 width,
			                                                            uint32 height,
			                                                            uint32 numMips,
			                                                            uint32 numSamples,
			                                                            uint32 numArraySlices,
			                                                            uint32 flags,
			                                                            RenderLayer::CEResourceState initialState,
			                                                            const RenderLayer::CEResourceData* initialData,
			                                                            const RenderLayer::CEClearValue&
				                                                            optimizedClearValue =
				                                                            RenderLayer::CEClearValue()) = 0;
			virtual RenderLayer::CETextureCube* CreateTextureCube(RenderLayer::CEFormat format,
			                                                      uint32 size,
			                                                      uint32 numMips,
			                                                      uint32 flags,
			                                                      RenderLayer::CEResourceState initialState,
			                                                      const RenderLayer::CEResourceData* initialData,
			                                                      const RenderLayer::CEClearValue& optimizedClearValue =
				                                                      RenderLayer::CEClearValue())
			= 0;
			virtual RenderLayer::CETextureCubeArray* CreateTextureCubeArray(RenderLayer::CEFormat format,
			                                                                uint32 size,
			                                                                uint32 numMips,
			                                                                uint32 numArraySlices,
			                                                                uint32 flags,
			                                                                RenderLayer::CEResourceState initialState,
			                                                                const RenderLayer::CEResourceData*
			                                                                initialData,
			                                                                const RenderLayer::CEClearValue&
				                                                                optimalClearValue =
				                                                                RenderLayer::CEClearValue()) = 0;
			virtual RenderLayer::CETexture3D* CreateTexture3D(RenderLayer::CEFormat format,
			                                                  uint32 width,
			                                                  uint32 height,
			                                                  uint32 depth,
			                                                  uint32 numMips,
			                                                  uint32 flags,
			                                                  RenderLayer::CEResourceState initialState,
			                                                  const RenderLayer::CEResourceData* initialData,
			                                                  const RenderLayer::CEClearValue& optimizedClearValue =
				                                                  RenderLayer::CEClearValue()) = 0;
			virtual class RenderLayer::CESamplerState* CreateSamplerState(
				const struct RenderLayer::CESamplerStateCreateInfo& createInfo) = 0;

			virtual RenderLayer::CEVertexBuffer* CreateVertexBuffer(uint32 stride,
			                                                        uint32 numVertices,
			                                                        uint32 flags,
			                                                        RenderLayer::CEResourceState initialState,
			                                                        const RenderLayer::CEResourceData* initialData) = 0;

			template <typename T>
			inline RenderLayer::CEVertexBuffer* CreateVertexBuffer(uint32 numVertices, uint32 flags,
			                                                       RenderLayer::CEResourceState initialState,
			                                                       const RenderLayer::CEResourceData* initialData) {
				constexpr uint32 stride = sizeof(T);
				return CreateVertexBuffer(stride, numVertices, flags, initialState, initialData);
			}

			virtual RenderLayer::CEIndexBuffer* CreateIndexBuffer(RenderLayer::CEIndexFormat format,
			                                                      uint32 numIndices,
			                                                      uint32 flags,
			                                                      RenderLayer::CEResourceState initialState,
			                                                      const RenderLayer::CEResourceData* initialData) = 0;
			virtual RenderLayer::CEConstantBuffer* CreateConstantBuffer(uint32 size,
			                                                            uint32 flags,
			                                                            RenderLayer::CEResourceState initialState,
			                                                            const RenderLayer::CEResourceData* initialData)
			= 0;

			template <typename T>
			inline RenderLayer::CEConstantBuffer* CreateConstantBuffer(uint32 flags,
			                                                           RenderLayer::CEResourceState initialState,
			                                                           const RenderLayer::CEResourceData* initialData) {
				constexpr uint32 sizeInBytes = sizeof(T);
				return CreateConstantBuffer(sizeInBytes, flags, initialState, initialData);
			}

			virtual RenderLayer::CEStructuredBuffer* CreateStructuredBuffer(uint32 stride,
			                                                                uint32 numElements,
			                                                                uint32 flags,
			                                                                RenderLayer::CEResourceState initialState,
			                                                                const RenderLayer::CEResourceData*
			                                                                initialData) = 0;

			virtual RenderLayer::CERayTracingScene* CreateRayTracingScene(uint32 flags,
			                                                              RenderLayer::CERayTracingGeometryInstance*
			                                                              instances,
			                                                              uint32 numInstances) = 0;
			virtual RenderLayer::CERayTracingGeometry* CreateRayTracingGeometry(uint32 flags,
				RenderLayer::CEVertexBuffer* vertexBuffer,
				RenderLayer::CEIndexBuffer* indexBuffer) = 0;

			virtual RenderLayer::CEShaderResourceView* CreateShaderResourceView(
				const RenderLayer::CEShaderResourceViewCreateInfo& createInfo) =
			0;

			inline RenderLayer::CEShaderResourceView* CreateShaderResourceViewForTexture2D(
				RenderLayer::CETexture2D* texture, RenderLayer::CEFormat format, uint32 mip, uint32 numMips,
				float minMipBias) {
				RenderLayer::CEShaderResourceViewCreateInfo createInfo(
					RenderLayer::CEShaderResourceViewCreateInfo::CEType::Texture2D);
				createInfo.Texture2D.Texture = texture;
				createInfo.Texture2D.Format = format;
				createInfo.Texture2D.Mip = mip;
				createInfo.Texture2D.NumMips = numMips;
				createInfo.Texture2D.MinMipsBias = minMipBias;
				return CreateShaderResourceView(createInfo);
			}

			inline RenderLayer::CEShaderResourceView* CreateShaderResourceViewForTexture2DArray(
				RenderLayer::CETexture2DArray* texture, RenderLayer::CEFormat format, uint32 mip, uint32 numMips,
				uint32 arraySlice, uint32 numArraySlices,
				float minMipBias) {
				RenderLayer::CEShaderResourceViewCreateInfo createInfo(
					RenderLayer::CEShaderResourceViewCreateInfo::CEType::Texture2DArray);
				createInfo.Texture2DArray.Texture = texture;
				createInfo.Texture2DArray.Format = format;
				createInfo.Texture2DArray.Mip = mip;
				createInfo.Texture2DArray.NumMips = numMips;
				createInfo.Texture2DArray.ArraySlice = arraySlice;
				createInfo.Texture2DArray.NumArraySlices = numArraySlices;
				createInfo.Texture2DArray.MinMipBias = minMipBias;
				return CreateShaderResourceView(createInfo);
			}

			inline RenderLayer::CEShaderResourceView* CreateShaderResourceViewForTextureCube(
				RenderLayer::CETextureCube* texture, RenderLayer::CEFormat format, uint32 mip, uint32 numMips,
				float minMipBias) {
				RenderLayer::CEShaderResourceViewCreateInfo createInfo(
					RenderLayer::CEShaderResourceViewCreateInfo::CEType::TextureCube);
				createInfo.TextureCube.Texture = texture;
				createInfo.TextureCube.Format = format;
				createInfo.TextureCube.Mip = mip;
				createInfo.TextureCube.NumMips = numMips;
				createInfo.TextureCube.MinMipBias = minMipBias;
				return CreateShaderResourceView(createInfo);
			}

			inline RenderLayer::CEShaderResourceView* CreateShaderResourceViewForTextureCubeArray(
				RenderLayer::CETextureCubeArray* texture, RenderLayer::CEFormat format, uint32 mip, uint32 numMips,
				uint32 arraySlice, uint32 numArraySlices, float minMipBias) {
				RenderLayer::CEShaderResourceViewCreateInfo createInfo(
					RenderLayer::CEShaderResourceViewCreateInfo::CEType::TextureCubeArray);
				createInfo.TextureCubeArray.Texture = texture;
				createInfo.TextureCubeArray.Format = format;
				createInfo.TextureCubeArray.Mip = mip;
				createInfo.TextureCubeArray.NumMips = numMips;
				createInfo.TextureCubeArray.ArraySlice = arraySlice;
				createInfo.TextureCubeArray.NumArraySlices = numArraySlices;
				createInfo.TextureCubeArray.MinMipBias = minMipBias;
				return CreateShaderResourceView(createInfo);
			}

			inline RenderLayer::CEShaderResourceView* CreateShaderResourceViewForTexture3D(
				RenderLayer::CETexture3D* texture, RenderLayer::CEFormat format, uint32 mip, uint32 numMips,
				uint32 depthSlice, uint32 numDepthSlices, float minMipBias) {
				RenderLayer::CEShaderResourceViewCreateInfo createInfo(
					RenderLayer::CEShaderResourceViewCreateInfo::CEType::Texture3D);
				createInfo.Texture3D.Texture = texture;
				createInfo.Texture3D.Format = format;
				createInfo.Texture3D.Mip = mip;
				createInfo.Texture3D.NumMips = numMips;
				createInfo.Texture3D.DepthSlice = depthSlice;
				createInfo.Texture3D.NumDepthSlice = numDepthSlices;
				createInfo.Texture3D.MinMipBias = minMipBias;
				return CreateShaderResourceView(createInfo);
			}

			inline RenderLayer::CEShaderResourceView* CreateShaderResourceViewForVertexBuffer(
				RenderLayer::CEVertexBuffer* buffer, uint32 firstVertex, uint32 numVertices) {
				RenderLayer::CEShaderResourceViewCreateInfo createInfo(
					RenderLayer::CEShaderResourceViewCreateInfo::CEType::VertexBuffer);
				createInfo.VertexBuffer.Buffer = buffer;
				createInfo.VertexBuffer.FirstVertex = firstVertex;
				createInfo.VertexBuffer.NumVertices = numVertices;
				return CreateShaderResourceView(createInfo);
			}

			inline RenderLayer::CEShaderResourceView* CreateShaderResourceViewForIndexBuffer(
				RenderLayer::CEIndexBuffer* buffer, uint32 firstIndex, uint32 numIndices) {
				RenderLayer::CEShaderResourceViewCreateInfo createInfo(
					RenderLayer::CEShaderResourceViewCreateInfo::CEType::IndexBuffer);
				createInfo.IndexBuffer.Buffer = buffer;
				createInfo.IndexBuffer.FirstIndex = firstIndex;
				createInfo.IndexBuffer.NumIndices = numIndices;
				return CreateShaderResourceView(createInfo);
			}

			inline RenderLayer::CEShaderResourceView* CreateShaderResourceViewForStructuredBuffer(
				RenderLayer::CEStructuredBuffer* buffer, uint32 firstElement, uint32 numElements) {
				RenderLayer::CEShaderResourceViewCreateInfo createInfo(
					RenderLayer::CEShaderResourceViewCreateInfo::CEType::StructuredBuffer);
				createInfo.StructuredBuffer.Buffer = buffer;
				createInfo.StructuredBuffer.FirstElement = firstElement;
				createInfo.StructuredBuffer.NumElements = numElements;
				return CreateShaderResourceView(createInfo);
			}

			virtual RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessView(
				const RenderLayer::CEUnorderedAccessViewCreateInfo& createInfo)
			= 0;

			inline RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessViewForTexture2D(
				RenderLayer::CETexture2D* texture, RenderLayer::CEFormat format, uint32 mip) {
				RenderLayer::CEUnorderedAccessViewCreateInfo createInfo(
					RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::Texture2D);
				createInfo.Texture2D.Texture = texture;
				createInfo.Texture2D.Format = format;
				createInfo.Texture2D.Mip = mip;
				return CreateUnorderedAccessView(createInfo);
			}

			inline RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessViewForTexture2DArray(
				RenderLayer::CETexture2DArray* texture, RenderLayer::CEFormat format, uint32 mip,
				uint32 arraySlice, uint32 numArraySlices) {
				RenderLayer::CEUnorderedAccessViewCreateInfo createInfo(
					RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::Texture2DArray);
				createInfo.Texture2DArray.Texture = texture;
				createInfo.Texture2DArray.Format = format;
				createInfo.Texture2DArray.Mip = mip;
				createInfo.Texture2DArray.ArraySlice = arraySlice;
				createInfo.Texture2DArray.NumArraySlices = numArraySlices;
				return CreateUnorderedAccessView(createInfo);
			}

			inline RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessViewForTextureCube(
				RenderLayer::CETextureCube* texture, RenderLayer::CEFormat format, uint32 mip) {
				RenderLayer::CEUnorderedAccessViewCreateInfo createInfo(
					RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::TextureCube);
				createInfo.TextureCube.Texture = texture;
				createInfo.TextureCube.Format = format;
				createInfo.TextureCube.Mip = mip;
				return CreateUnorderedAccessView(createInfo);
			}

			inline RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessViewForTextureCubeArray(
				RenderLayer::CETextureCubeArray* texture, RenderLayer::CEFormat format, uint32 mip,
				uint32 arraySlice, uint32 numArraySlices) {
				RenderLayer::CEUnorderedAccessViewCreateInfo createInfo(
					RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::TextureCubeArray);
				createInfo.TextureCubeArray.Texture = texture;
				createInfo.TextureCubeArray.Format = format;
				createInfo.TextureCubeArray.Mip = mip;
				createInfo.TextureCubeArray.ArraySlice = arraySlice;
				createInfo.TextureCubeArray.NumArraySlices = numArraySlices;
				return CreateUnorderedAccessView(createInfo);
			}

			inline RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessViewForTexture3D(
				RenderLayer::CETexture3D* texture, RenderLayer::CEFormat format, uint32 mip,
				uint32 depthSlice, uint32 numDepthSlices) {
				RenderLayer::CEUnorderedAccessViewCreateInfo createInfo(
					RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::Texture3D);
				createInfo.Texture3D.Texture = texture;
				createInfo.Texture3D.Format = format;
				createInfo.Texture3D.Mip = mip;
				createInfo.Texture3D.DepthSlice = depthSlice;
				createInfo.Texture3D.NumDepthSlices = numDepthSlices;
				return CreateUnorderedAccessView(createInfo);
			}

			inline RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessViewForVertexBuffer(
				RenderLayer::CEVertexBuffer* buffer, uint32 firstVertex, uint32 numVertices) {
				RenderLayer::CEUnorderedAccessViewCreateInfo createInfo(
					RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::VertexBuffer);
				createInfo.VertexBuffer.Buffer = buffer;
				createInfo.VertexBuffer.FirstVertex = firstVertex;
				createInfo.VertexBuffer.NumVertices = numVertices;
				return CreateUnorderedAccessView(createInfo);
			}

			inline RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessViewForIndexBuffer(
				RenderLayer::CEIndexBuffer* buffer, uint32 firstIndex, uint32 numIndices) {
				RenderLayer::CEUnorderedAccessViewCreateInfo createInfo(
					RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::IndexBuffer);
				createInfo.IndexBuffer.Buffer = buffer;
				createInfo.IndexBuffer.FirstIndex = firstIndex;
				createInfo.IndexBuffer.NumIndices = numIndices;
				return CreateUnorderedAccessView(createInfo);
			}

			inline RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessViewForStructuredBuffer(
				RenderLayer::CEStructuredBuffer* buffer, uint32 firstElement, uint32 numElements) {
				RenderLayer::CEUnorderedAccessViewCreateInfo createInfo(
					RenderLayer::CEUnorderedAccessViewCreateInfo::CEType::StructuredBuffer);
				createInfo.StructuredBuffer.Buffer = buffer;
				createInfo.StructuredBuffer.FirstElement = firstElement;
				createInfo.StructuredBuffer.NumElements = numElements;
				return CreateUnorderedAccessView(createInfo);
			}

			virtual RenderLayer::CERenderTargetView* CreateRenderTargetView(
				const RenderLayer::CERenderTargetViewCreateInfo& createInfo) = 0;


			inline RenderLayer::CERenderTargetView* CreateRenderTargetViewForTexture2D(
				RenderLayer::CETexture2D* texture, RenderLayer::CEFormat format, uint32 mip) {
				RenderLayer::CERenderTargetViewCreateInfo createInfo(
					RenderLayer::CERenderTargetViewCreateInfo::CEType::Texture2D);
				createInfo.Texture2D.Texture = texture;
				createInfo.Format = format;
				createInfo.Texture2D.Mip = mip;
				return CreateRenderTargetView(createInfo);
			}

			inline RenderLayer::CERenderTargetView* CreateRenderTargetViewForTexture2DArray(
				RenderLayer::CETexture2DArray* texture, RenderLayer::CEFormat format, uint32 mip,
				uint32 arraySlice, uint32 numArraySlices) {
				RenderLayer::CERenderTargetViewCreateInfo createInfo(
					RenderLayer::CERenderTargetViewCreateInfo::CEType::Texture2DArray);
				createInfo.Texture2DArray.Texture = texture;
				createInfo.Format = format;
				createInfo.Texture2DArray.Mip = mip;
				createInfo.Texture2DArray.ArraySlice = arraySlice;
				createInfo.Texture2DArray.NumArraySlices = numArraySlices;
				return CreateRenderTargetView(createInfo);
			}

			inline RenderLayer::CERenderTargetView* CreateRenderTargetViewForTextureCube(
				RenderLayer::CETextureCube* texture, RenderLayer::CEFormat format, uint32 mip,
				RenderLayer::CECubeFace cubeFace) {
				RenderLayer::CERenderTargetViewCreateInfo createInfo(
					RenderLayer::CERenderTargetViewCreateInfo::CEType::TextureCube);
				createInfo.TextureCube.Texture = texture;
				createInfo.Format = format;
				createInfo.TextureCube.Mip = mip;
				createInfo.TextureCube.CubeFace = cubeFace;
				return CreateRenderTargetView(createInfo);
			}

			inline RenderLayer::CERenderTargetView* CreateRenderTargetViewForTextureCubeArray(
				RenderLayer::CETextureCubeArray* texture, RenderLayer::CEFormat format, uint32 mip,
				uint32 arraySlice, RenderLayer::CECubeFace cubeFace) {
				RenderLayer::CERenderTargetViewCreateInfo createInfo(
					RenderLayer::CERenderTargetViewCreateInfo::CEType::TextureCubeArray);
				createInfo.TextureCubeArray.Texture = texture;
				createInfo.Format = format;
				createInfo.TextureCubeArray.Mip = mip;
				createInfo.TextureCubeArray.ArraySlice = arraySlice;
				createInfo.TextureCubeArray.CubeFace = cubeFace;
				return CreateRenderTargetView(createInfo);
			}

			inline RenderLayer::CERenderTargetView* CreateRenderTargetViewForTexture3D(
				RenderLayer::CETexture3D* texture, RenderLayer::CEFormat format, uint32 mip,
				uint32 depthSlice, uint32 numDepthSlices) {
				RenderLayer::CERenderTargetViewCreateInfo createInfo(
					RenderLayer::CERenderTargetViewCreateInfo::CEType::Texture3D);
				createInfo.Texture3D.Texture = texture;
				createInfo.Format = format;
				createInfo.Texture3D.Mip = mip;
				createInfo.Texture3D.DepthSlice = depthSlice;
				createInfo.Texture3D.NumDepthSlices = numDepthSlices;
				return CreateRenderTargetView(createInfo);
			}

			virtual RenderLayer::CEDepthStencilView* CreateDepthStencilView(
				const RenderLayer::CEDepthStencilViewCreateInfo& createInfo) = 0;


			inline RenderLayer::CEDepthStencilView* CreateDepthStencilViewForTexture2D(
				RenderLayer::CETexture2D* texture, RenderLayer::CEFormat format, uint32 mip, uint32 numMips,
				float minMipBias) {
				RenderLayer::CEDepthStencilViewCreateInfo createInfo(
					RenderLayer::CEDepthStencilViewCreateInfo::CEType::Texture2D);
				createInfo.Texture2D.Texture = texture;
				createInfo.Format = format;
				createInfo.Texture2D.Mip = mip;
				return CreateDepthStencilView(createInfo);
			}

			inline RenderLayer::CEDepthStencilView* CreateDepthStencilViewForTexture2DArray(
				RenderLayer::CETexture2DArray* texture, RenderLayer::CEFormat format, uint32 mip, uint32 numMips,
				uint32 arraySlice, uint32 numArraySlices,
				float minMipBias) {
				RenderLayer::CEDepthStencilViewCreateInfo createInfo(
					RenderLayer::CEDepthStencilViewCreateInfo::CEType::Texture2DArray);
				createInfo.Texture2DArray.Texture = texture;
				createInfo.Format = format;
				createInfo.Texture2DArray.Mip = mip;
				createInfo.Texture2DArray.ArraySlice = arraySlice;
				createInfo.Texture2DArray.NumArraySlices = numArraySlices;
				return CreateDepthStencilView(createInfo);
			}

			inline RenderLayer::CEDepthStencilView* CreateDepthStencilViewForTextureCube(
				RenderLayer::CETextureCube* texture, RenderLayer::CEFormat format, uint32 mip,
				RenderLayer::CECubeFace cubeFace) {
				RenderLayer::CEDepthStencilViewCreateInfo createInfo(
					RenderLayer::CEDepthStencilViewCreateInfo::CEType::TextureCube);
				createInfo.TextureCube.Texture = texture;
				createInfo.Format = format;
				createInfo.TextureCube.Mip = mip;
				createInfo.TextureCube.CubeFace = cubeFace;
				return CreateDepthStencilView(createInfo);
			}

			inline RenderLayer::CEDepthStencilView* CreateDepthStencilViewForTextureCubeArray(
				RenderLayer::CETextureCubeArray* texture, RenderLayer::CEFormat format, uint32 mip,
				uint32 arraySlice, RenderLayer::CECubeFace cubeFace) {
				RenderLayer::CEDepthStencilViewCreateInfo createInfo(
					RenderLayer::CEDepthStencilViewCreateInfo::CEType::TextureCubeArray);
				createInfo.TextureCubeArray.Texture = texture;
				createInfo.Format = format;
				createInfo.TextureCubeArray.Mip = mip;
				createInfo.TextureCubeArray.ArraySlice = arraySlice;
				createInfo.TextureCubeArray.CubeFace = cubeFace;
				return CreateDepthStencilView(createInfo);
			}

			virtual class RenderLayer::CEComputeShader* CreateComputeShader(
				const CEArray<uint8>& shaderCode) = 0;

			virtual class RenderLayer::CEVertexShader* CreateVertexShader(
				const CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CEHullShader* CreateHullShader(
				const CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CEDomainShader* CreateDomainShader(
				const CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CEGeometryShader* CreateGeometryShader(
				const CEArray<uint8>& shaderCode) =
			0;
			virtual class RenderLayer::CEMeshShader* CreateMeshShader(
				const CEArray<uint8>& shaderCode) =
			0;
			virtual class RenderLayer::CEAmplificationShader* CreateAmplificationShader(
				const CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CEPixelShader* CreatePixelShader(
				const CEArray<uint8>& shaderCode) = 0;

			virtual class RenderLayer::CERayGenShader* CreateRayGenShader(
				const CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CERayAnyHitShader* CreateRayAnyHitShader(
				const CEArray<uint8>& shaderCode) =
			0;
			virtual class RenderLayer::CERayClosestHitShader* CreateRayClosestHitShader(
				const CEArray<uint8>& shaderCode)
			=
			0;
			virtual class RenderLayer::CERayMissShader* CreateRayMissShader(
				const CEArray<uint8>& shaderCode) = 0;

			virtual class RenderLayer::CEDepthStencilState* CreateDepthStencilState(
				const RenderLayer::CEDepthStencilStateCreateInfo& createInfo)
			= 0;

			virtual class RenderLayer::CERasterizerState* CreateRasterizerState(
				const RenderLayer::CERasterizerStateCreateInfo& createInfo) = 0;

			virtual class RenderLayer::CEBlendState* CreateBlendState(
				const RenderLayer::CEBlendStateCreateInfo& createInfo) = 0;

			virtual class RenderLayer::CEInputLayoutState* CreateInputLayout(
				const RenderLayer::CEInputLayoutStateCreateInfo& createInfo) = 0;

			virtual class RenderLayer::CEGraphicsPipelineState* CreateGraphicsPipelineState(
				const RenderLayer::CEGraphicsPipelineStateCreateInfo& createInfo) = 0;
			virtual class RenderLayer::CEComputePipelineState* CreateComputePipelineState(
				const RenderLayer::CEComputePipelineStateCreateInfo& createInfo) = 0;
			virtual class RenderLayer::CERayTracingPipelineState* CreateRayTracingPipelineState(
				const RenderLayer::CERayTracingPipelineStateCreateInfo& createInfo) = 0;

			virtual class RenderLayer::CEGPUProfiler* CreateProfiler() = 0;

			virtual class RenderLayer::CEViewport* CreateViewport(Core::Platform::Generic::Window::CEWindow* window,
			                                                      uint32 width,
			                                                      uint32 height, RenderLayer::CEFormat colorFormat,
			                                                      RenderLayer::CEFormat depthFormat) = 0;

			virtual class RenderLayer::CEICommandContext* GetDefaultCommandContext() = 0;

			virtual std::string GetAdapterName() {
				return std::string();
			}

			virtual void CheckRayTracingSupport(CERayTracingSupport& outSupport) = 0;
			virtual void CheckShadingRateSupport(CEShadingRateSupport& outSupport) = 0;

			virtual bool UAVSupportsFormat(RenderLayer::CEFormat format) {
				(void)format;
				return false;
			}

			inline bool IsRayTracingSupported() {
				CERayTracingSupport support;
				CheckRayTracingSupport(support);

				return support.Tier != CERayTracingTier::NotSupported;
			}

			inline bool IsShadingRateSupported() {
				CEShadingRateSupport support;
				CheckShadingRateSupport(support);

				return support.Tier != CEShadingRateTier::NotSupported;
			}
		};
	}}
