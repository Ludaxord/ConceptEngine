#pragma once

#include <string>

#include "../../../Core/Common/CEManager.h"
#include "../RenderLayer/CETexture.h"
#include "../RenderLayer/CERendering.h"
#include "../RenderLayer/CEBuffer.h"
#include "../RenderLayer/CERayTracing.h"
#include "../RenderLayer/CEResourceViews.h"
#include "../RenderLayer/CESamplerState.h"
#include "../RenderLayer/CEPipelineState.h"
#include "../../../Core/Containers/CEArray.h"
#include "../../../Core/Platform/Generic/Window/CEWindow.h"
#include "../RenderLayer/CEGPUProfiler.h"
#include "../RenderLayer/CEViewport.h"
#include "../RenderLayer/CEICommandContext.h"
#include "../../../Core/Common/CETypes.h"

#define ENABLE_API_DEBUGGING 0;
#define ENABLE_API_GPU_DEBUGGING 0;
#define ENABLE_API_GPU_BREADCRUMBS 0;

namespace ConceptEngine::Graphics::Main {

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
			~CEGraphicsManager() override = default;

			virtual bool Create() override;
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
			virtual RenderLayer::CEIndexBuffer* CreateIndexBuffer(RenderLayer::CEFormat format,
			                                                      uint32 numIndices,
			                                                      uint32 flags,
			                                                      RenderLayer::CEResourceState initialState,
			                                                      const RenderLayer::CEResourceData* initialData) = 0;
			virtual RenderLayer::CEConstantBuffer* CreateConstantBuffer(uint32 size,
			                                                            uint32 flags,
			                                                            RenderLayer::CEResourceState initialState,
			                                                            const RenderLayer::CEResourceData* initialData)
			= 0;
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
			virtual RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessView(
				const RenderLayer::CEUnorderedAccessViewCreateInfo& createInfo)
			= 0;
			virtual RenderLayer::CERenderTargetView* CreateRenderTargetView(
				const RenderLayer::CERenderTargetViewCreateInfo& createInfo) = 0;
			virtual RenderLayer::CEDepthStencilView* CreateDepthStencilView(
				const RenderLayer::CEDepthStencilViewCreateInfo& createInfo) = 0;

			virtual class RenderLayer::CEComputeShader* CreateComputeShader(
				const Core::Containers::CEArray<uint8>& shaderCode) = 0;

			virtual class RenderLayer::CEVertexShader* CreateVertexShader(
				const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CEHullShader* CreateHullShader(
				const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CEDomainShader* CreateDomainShader(
				const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CEGeometryShader* CreateGeometryShader(
				const Core::Containers::CEArray<uint8>& shaderCode) =
			0;
			virtual class RenderLayer::CEAmplificationShader* CreateAmplificationShader(
				const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CEPixelShader* CreatePixelShader(
				const Core::Containers::CEArray<uint8>& shaderCode) = 0;

			virtual class RenderLayer::CERayGenShader* CreateRayGenShader(
				const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class RenderLayer::CERayAnyHitShader* CreateRayAnyHitShader(
				const Core::Containers::CEArray<uint8>& shaderCode) =
			0;
			virtual class RenderLayer::CERayClosestHitShader* CreateClosestHitShader(
				const Core::Containers::CEArray<uint8>& shaderCode)
			=
			0;
			virtual class RenderLayer::CERayMissShader* CreateRayMissShader(
				const Core::Containers::CEArray<uint8>& shaderCode) = 0;

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
			virtual class RenderLayer::CERayTracingPipelineState* CreatRayTracingPipelineState(
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

			template <typename T>
			RenderLayer::CEVertexBuffer* CreateVertexBuffer(uint32 NumVertices, uint32 Flags,
			                                                RenderLayer::CEResourceState InitialState,
			                                                const RenderLayer::CEResourceData* InitialData) {
				constexpr uint32 STRIDE = sizeof(T);
				return CreateVertexBuffer(STRIDE, NumVertices, Flags, InitialState, InitialData);
			}


			template <typename T>
			RenderLayer::CEConstantBuffer* CreateConstantBuffer(uint32 Flags, RenderLayer::CEResourceState InitialState,
			                                                    const RenderLayer::CEResourceData* InitialData) {
				constexpr uint32 SIZE_IN_BYTES = sizeof(T);
				return CreateConstantBuffer(SIZE_IN_BYTES, Flags, InitialState, InitialData);
			}

			//TODO: implement all views creators as inline methods
			inline RenderLayer::CEShaderResourceView* CreateShaderResourceView(
				RenderLayer::CETexture2D* texture, RenderLayer::CEFormat format, uint32 mip, uint32 numMips,
				float minMipBias);

			inline RenderLayer::CEUnorderedAccessView* CreateUnorderedAccessView(
				RenderLayer::CETextureCube* texture, RenderLayer::CEFormat format, uint32 mip);
		};
	}}
