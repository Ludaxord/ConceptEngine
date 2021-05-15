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
#include "../RenderLayer/CEShaderCompiler.h"


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

		using namespace RenderLayer;

		class CEGraphicsManager : public Core::Common::CEManager {
		public:
			CEGraphicsManager(): CEManager() {
			};
			~CEGraphicsManager() override = default;

			virtual bool Create() override;
			virtual void Destroy() = 0;

			virtual CETexture2D* CreateTexture2D(CEFormat format,
			                                     uint32 width,
			                                     uint32 height,
			                                     uint32 numMips,
			                                     uint32 numSamples,
			                                     uint32 flags,
			                                     CEResourceState initialState,
			                                     const CEResourceData* initialData,
			                                     const CEClearValue& optimizedClearValue) = 0;
			virtual CETexture2DArray* CreateTexture2DArray(CEFormat format,
			                                               uint32 width,
			                                               uint32 height,
			                                               uint32 numMips,
			                                               uint32 numSamples,
			                                               uint32 numArraySlices,
			                                               uint32 flags,
			                                               CEResourceState initialState,
			                                               const CEResourceData* initialData,
			                                               const CEClearValue&
			                                               optimizedClearValue) = 0;
			virtual CETextureCube* CreateTextureCube(CEFormat format,
			                                         uint32 width,
			                                         uint32 height,
			                                         uint32 depth,
			                                         uint32 numMips,
			                                         uint32 flags,
			                                         CEResourceState initialState,
			                                         const CEResourceData* initialData,
			                                         const CEClearValue& optimizedClearValue) = 0;
			virtual CETextureCubeArray* CreateTextureCubeArray(CEFormat format,
			                                                   uint32 width,
			                                                   uint32 height,
			                                                   uint32 depth,
			                                                   uint32 numMips,
			                                                   uint32 flags,
			                                                   CEResourceState initialState,
			                                                   const CEResourceData* initialData,
			                                                   const CEClearValue& optimalClearValue) = 0;
			virtual CETexture3D* CreateTexture3D(CEFormat format,
			                                     uint32 width,
			                                     uint32 height,
			                                     uint32 depth,
			                                     uint32 numMips,
			                                     uint32 flags,
			                                     CEResourceState initialState,
			                                     const CEResourceData* initialData,
			                                     const CEClearValue* optimizedClearValue) = 0;

			virtual class CESamplerState* CreateSamplerState(const struct CESamplerStateCreateInfo& createInfo) = 0;

			virtual CEVertexBuffer* CreateVertexBuffer(uint32 stride,
			                                           uint32 numVertices,
			                                           uint32 flags,
			                                           CEResourceState initialState,
			                                           const CEResourceData* initialData) = 0;
			virtual CEIndexBuffer* CreateIndexBuffer(CEFormat format,
			                                         uint32 numIndices,
			                                         uint32 flags,
			                                         CEResourceState initialState,
			                                         const CEResourceData* initialData) = 0;
			virtual CEConstantBuffer* CreateConstantBuffer(uint32 size,
			                                               uint32 flags,
			                                               CEResourceState initialState,
			                                               const CEResourceData* initialData) = 0;
			virtual CEStructuredBuffer* CreateStructuredBuffer(uint32 stride,
			                                                   uint32 numElements,
			                                                   uint32 flags,
			                                                   CEResourceState initialState,
			                                                   const CEResourceData* initialData) = 0;

			virtual CERayTracingScene* CreateRayTracingScene(uint32 flags,
			                                                 CERayTracingGeometryInstance* instances,
			                                                 uint32 numInstances) = 0;
			virtual CERayTracingGeometry* CreateRayTracingGeometry(uint32 flags,
			                                                       CEVertexBuffer* vertexBuffer,
			                                                       CEIndexBuffer* indexBuffer) = 0;

			virtual CEShaderResourceView* CreateShaderResourceView(const CEShaderResourceViewCreateInfo& createInfo) =
			0;
			virtual CEUnorderedAccessView* CreateUnorderedAccessView(const CEUnorderedAccessViewCreateInfo& createInfo)
			= 0;
			virtual CERenderTargetView* CreateRenderTargetView(const CERenderTargetViewCreateInfo& createInfo) = 0;
			virtual CEDepthStencilView* CreateDepthStencilView(const CEDepthStencilViewCreateInfo& createInfo) = 0;

			virtual class CEComputeShader* CreateComputeShader(const Core::Containers::CEArray<uint8>& shaderCode) = 0;

			virtual class CEVertexShader* CreateVertexShader(const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class CEHullShader* CreateHullShader(const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class CEDomainShader* CreateDomainShader(const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class CEGeometryShader* CreateGeometryShader(const Core::Containers::CEArray<uint8>& shaderCode) =
			0;
			virtual class CEAmplificationShader* CreateAmplificationShader(
				const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class CEPixelShader* CreatePixelShader(const Core::Containers::CEArray<uint8>& shaderCode) = 0;

			virtual class CERayGenShader* CreateRayGenShader(const Core::Containers::CEArray<uint8>& shaderCode) = 0;
			virtual class CERayGenHitShader* CreateRayGenHitShader(const Core::Containers::CEArray<uint8>& shaderCode) =
			0;
			virtual class CEClosestHitShader* CreateClosestHitShader(const Core::Containers::CEArray<uint8>& shaderCode)
			=
			0;
			virtual class CERayMissShader* CreateRayMissShader(const Core::Containers::CEArray<uint8>& shaderCode) = 0;

			virtual class CEDepthStencilState* CreateDepthStencilState(const CEDepthStencilStateCreateInfo& createInfo)
			= 0;

			virtual class CERasterizerState* CreateRasterizerState(const CERasterizerStateCreateInfo& createInfo) = 0;

			virtual class CEBlendState* CreateBlendState(const CEBlendStateCreateInfo& createInfo) = 0;

			virtual class CEInputLayoutState* CreateInputLayout(const CEInputLayoutStateCreateInfo& createInfo) = 0;

			virtual class CEGraphicsPipelineState* CreateGraphicsPipelineState(
				const CEGraphicsPipelineStateCreateInfo& createInfo) = 0;
			virtual class CEComputePipelineState* CreateComputePipelineState(
				const CEComputePipelineStateCreateInfo& createInfo) = 0;
			virtual class CERayTracingPipelineState* CreatRayTracingPipelineState(
				const CERayTracingPipelineStateCreateInfo& createInfo) = 0;

			virtual class CEGPUProfiler* CreateProfiler() = 0;

			virtual class CEViewport* CreateViewport(Core::Platform::Generic::Window::CEWindow* window, uint32 width,
			                                         uint32 height, CEFormat colorFormat, CEFormat depthFormat) = 0;

			virtual class CEICommandContext* GetDefaultCommandContext() = 0;

			virtual std::string GetAdapterName() {
				return std::string();
			}

			virtual void CheckRayTracingSupport(CERayTracingSupport& outSupport) = 0;
			virtual void CheckShadingRateSupport(CEShadingRateSupport& outSupport) = 0;

			virtual bool UAVSupportsFormat(CEFormat format) {
				(void)format;
				return false;
			}
		};
	}}
