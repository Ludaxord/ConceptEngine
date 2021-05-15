#pragma once

#include "../../../Main/Managers/CEGraphicsManager.h"

namespace ConceptEngine::Graphics {

	using namespace Main::RenderLayer;
	using namespace Main::Managers;

	namespace DirectX12::Modules::Managers {

		class CEDXManager final : public CEGraphicsManager {
		public:
			CEDXManager();
			~CEDXManager() override;

			bool Create() override;
			void Destroy() override;

			CETexture2D* CreateTexture2D(CEFormat format, uint32 width, uint32 height, uint32 numMips,
			                             uint32 numSamples, uint32 flags, CEResourceState initialState,
			                             const CEResourceData* initialData,
			                             const CEClearValue& optimizedClearValue) override;
			CETexture2DArray* CreateTexture2DArray(CEFormat format, uint32 width, uint32 height, uint32 numMips,
			                                       uint32 numSamples, uint32 numArraySlices, uint32 flags,
			                                       CEResourceState initialState,
			                                       const CEResourceData* initialData,
			                                       const CEClearValue& optimizedClearValue) override;
			CETextureCube* CreateTextureCube(CEFormat format, uint32 width, uint32 height, uint32 depth, uint32 numMips,
			                                 uint32 flags, CEResourceState initialState,
			                                 const CEResourceData* initialData,
			                                 const CEClearValue& optimizedClearValue) override;
			CETextureCubeArray* CreateTextureCubeArray(CEFormat format, uint32 width, uint32 height, uint32 depth,
			                                           uint32 numMips, uint32 flags, CEResourceState initialState,
			                                           const CEResourceData* initialData,
			                                           const CEClearValue& optimalClearValue) override;
			CETexture3D* CreateTexture3D(CEFormat format, uint32 width, uint32 height, uint32 depth, uint32 numMips,
			                             uint32 flags, CEResourceState initialState, const CEResourceData* initialData,
			                             const CEClearValue* optimizedClearValue) override;
			
			CESamplerState* CreateSamplerState(const CESamplerStateCreateInfo& createInfo) override;
			
			CEVertexBuffer* CreateVertexBuffer(uint32 stride, uint32 numVertices, uint32 flags,
			                                   CEResourceState initialState,
			                                   const CEResourceData* initialData) override;
			CEIndexBuffer* CreateIndexBuffer(CEFormat format, uint32 numIndices, uint32 flags,
			                                 CEResourceState initialState, const CEResourceData* initialData) override;
			CEConstantBuffer* CreateConstantBuffer(uint32 size, uint32 flags, CEResourceState initialState,
			                                       const CEResourceData* initialData) override;
			CEStructuredBuffer* CreateStructuredBuffer(uint32 stride, uint32 numElements, uint32 flags,
			                                           CEResourceState initialState,
			                                           const CEResourceData* initialData) override;
			
			CERayTracingScene* CreateRayTracingScene(uint32 flags, CERayTracingGeometryInstance* instances,
			                                         uint32 numInstances) override;
			CERayTracingGeometry* CreateRayTracingGeometry(uint32 flags, CEVertexBuffer* vertexBuffer,
			                                               CEIndexBuffer* indexBuffer) override;
			
			CEShaderResourceView* CreateShaderResourceView(const CEShaderResourceViewCreateInfo& createInfo) override;
			CEUnorderedAccessView*
			CreateUnorderedAccessView(const CEUnorderedAccessViewCreateInfo& createInfo) override;
			CERenderTargetView* CreateRenderTargetView(const CERenderTargetViewCreateInfo& createInfo) override;
			CEDepthStencilView* CreateDepthStencilView(const CEDepthStencilViewCreateInfo& createInfo) override;
			
			CEComputeShader* CreateComputeShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			CEVertexShader* CreateVertexShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			CEHullShader* CreateHullShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			CEDomainShader* CreateDomainShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			CEGeometryShader* CreateGeometryShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			CEAmplificationShader*
			CreateAmplificationShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			CEPixelShader* CreatePixelShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			
			CERayGenShader* CreateRayGenShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			CERayGenHitShader* CreateRayGenHitShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			CEClosestHitShader* CreateClosestHitShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			CERayMissShader* CreateRayMissShader(const Core::Containers::CEArray<uint8>& shaderCode) override;
			
			CEDepthStencilState* CreateDepthStencilState(const CEDepthStencilStateCreateInfo& createInfo) override;
			CERasterizerState* CreateRasterizerState(const CERasterizerStateCreateInfo& createInfo) override;
			CEBlendState* CreateBlendState(const CEBlendStateCreateInfo& createInfo) override;
			
			CEInputLayoutState* CreateInputLayout(const CEInputLayoutStateCreateInfo& createInfo) override;
			
			CEGraphicsPipelineState*
			CreateGraphicsPipelineState(const CEGraphicsPipelineStateCreateInfo& createInfo) override;
			CEComputePipelineState*
			CreateComputePipelineState(const CEComputePipelineStateCreateInfo& createInfo) override;
			
			CERayTracingPipelineState* CreatRayTracingPipelineState(
				const CERayTracingPipelineStateCreateInfo& createInfo) override;
			
			CEGPUProfiler* CreateProfiler() override;
			CEViewport* CreateViewport(Core::Platform::Generic::Window::CEWindow* window, uint32 width, uint32 height,
			                          CEFormat colorFormat, CEFormat depthFormat) override;
			
			CEICommandContext* GetDefaultCommandContext() override;
			
			void CheckRayTracingSupport(Main::CERayTracingSupport& outSupport) override;
			void CheckShadingRateSupport(Main::CEShadingRateSupport& outSupport) override;
		};
	}}
