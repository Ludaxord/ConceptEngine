#pragma once

#include "../../../Main/Managers/CEGraphicsManager.h"
#include "../RenderLayer/CEDXDescriptorHeap.h"
#include "../RenderLayer/CEDXDevice.h"
#include "../RenderLayer/CEDXCommandContext.h"
#include "../RenderLayer/CEDXRootSignature.h"

namespace ConceptEngine::Graphics {

	using namespace Main::RenderLayer;
	using namespace Main::Managers;

	namespace DirectX12::Modules::Managers {

		class CEDXManager final : public CEGraphicsManager {
		public:
			CEDXManager();
			~CEDXManager() override;

			virtual bool Create() override final;
			void Destroy() override;

			RenderLayer::CEDXOfflineDescriptorHeap* GetResourceOfflineDescriptorHeap() {
				return ResourceOfflineDescriptorHeap;
			}

			RenderLayer::CEDXOfflineDescriptorHeap* GetRenderTargetOfflineDescriptorHeap() {
				return RenderTargetOfflineDescriptorHeap;
			}

			RenderLayer::CEDXOfflineDescriptorHeap* GetDepthStencilOfflineDescriptorHeap() {
				return DepthStencilOfflineDescriptorHeap;
			}

			RenderLayer::CEDXOfflineDescriptorHeap* GetSamplerOfflineDescriptorHeap() {
				return SamplerOfflineDescriptorHeap;
			}


			virtual CETexture2D* CreateTexture2D(CEFormat format, uint32 width, uint32 height, uint32 numMips,
			                                     uint32 numSamples, uint32 flags, CEResourceState initialState,
			                                     const CEResourceData* initialData,
			                                     const CEClearValue& optimizedClearValue) override final;

			virtual CETexture2DArray* CreateTexture2DArray(CEFormat format, uint32 width, uint32 height, uint32 numMips,
			                                               uint32 numSamples, uint32 numArraySlices, uint32 flags,
			                                               CEResourceState initialState,
			                                               const CEResourceData* initialData,
			                                               const CEClearValue& optimizedClearValue) override final;

			virtual CETextureCube* CreateTextureCube(CEFormat format,
			                                         uint32 size,
			                                         uint32 numMips,
			                                         uint32 flags, CEResourceState initialState,
			                                         const CEResourceData* initialData,
			                                         const CEClearValue& optimizedClearValue) override final;

			virtual CETextureCubeArray* CreateTextureCubeArray(CEFormat format, 
			                                                   uint32 size,
			                                                   uint32 numMips,
			                                                   uint32 numArraySlices,
			                                                   uint32 flags,
			                                                   CEResourceState initialState,
			                                                   const CEResourceData* initialData,
			                                                   const CEClearValue& optimalClearValue) override final;

			virtual CETexture3D* CreateTexture3D(CEFormat format, uint32 width, uint32 height, uint32 depth,
			                                     uint32 numMips,
			                                     uint32 flags, CEResourceState initialState,
			                                     const CEResourceData* initialData,
			                                     const CEClearValue& optimizedClearValue) override final;

			virtual CESamplerState* CreateSamplerState(const CESamplerStateCreateInfo& createInfo) override final;

			virtual CEVertexBuffer* CreateVertexBuffer(uint32 stride, uint32 numVertices, uint32 flags,
			                                           CEResourceState initialState,
			                                           const CEResourceData* initialData) override final;

			virtual CEIndexBuffer* CreateIndexBuffer(CEFormat format, uint32 numIndices, uint32 flags,
			                                         CEResourceState initialState,
			                                         const CEResourceData* initialData) override final;

			virtual CEConstantBuffer* CreateConstantBuffer(uint32 size, uint32 flags, CEResourceState initialState,
			                                               const CEResourceData* initialData) override final;

			virtual CEStructuredBuffer* CreateStructuredBuffer(uint32 stride, uint32 numElements, uint32 flags,
			                                                   CEResourceState initialState,
			                                                   const CEResourceData* initialData) override final;

			virtual CERayTracingScene* CreateRayTracingScene(uint32 flags, CERayTracingGeometryInstance* instances,
			                                                 uint32 numInstances) override final;

			virtual CERayTracingGeometry* CreateRayTracingGeometry(uint32 flags, CEVertexBuffer* vertexBuffer,
			                                                       CEIndexBuffer* indexBuffer) override final;


			virtual CEShaderResourceView*
			CreateShaderResourceView(const CEShaderResourceViewCreateInfo& createInfo) override final;

			virtual CEUnorderedAccessView*
			CreateUnorderedAccessView(const CEUnorderedAccessViewCreateInfo& createInfo) override final;

			virtual CERenderTargetView*
			CreateRenderTargetView(const CERenderTargetViewCreateInfo& createInfo) override final;

			virtual CEDepthStencilView*
			CreateDepthStencilView(const CEDepthStencilViewCreateInfo& createInfo) override final;

			virtual CEComputeShader*
			CreateComputeShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
			virtual CEVertexShader*
			CreateVertexShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
			virtual CEHullShader* CreateHullShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
			virtual CEDomainShader*
			CreateDomainShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
			virtual CEGeometryShader*
			CreateGeometryShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;

			virtual CEAmplificationShader*
			CreateAmplificationShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
			virtual CEPixelShader* CreatePixelShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;

			virtual CERayGenShader*
			CreateRayGenShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
			virtual CERayGenHitShader*
			CreateRayGenHitShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
			virtual CEClosestHitShader*
			CreateClosestHitShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
			virtual CERayMissShader*
			CreateRayMissShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;

			virtual CEDepthStencilState*
			CreateDepthStencilState(const CEDepthStencilStateCreateInfo& createInfo) override final;
			virtual CERasterizerState*
			CreateRasterizerState(const CERasterizerStateCreateInfo& createInfo) override final;
			virtual CEBlendState* CreateBlendState(const CEBlendStateCreateInfo& createInfo) override final;

			virtual CEInputLayoutState*
			CreateInputLayout(const CEInputLayoutStateCreateInfo& createInfo) override final;

			virtual CEGraphicsPipelineState*
			CreateGraphicsPipelineState(const CEGraphicsPipelineStateCreateInfo& createInfo) override final;

			virtual CEComputePipelineState*
			CreateComputePipelineState(const CEComputePipelineStateCreateInfo& createInfo) override final;

			virtual CERayTracingPipelineState* CreatRayTracingPipelineState(
				const CERayTracingPipelineStateCreateInfo& createInfo) override final;

			virtual CEGPUProfiler* CreateProfiler() override final;
			virtual CEViewport* CreateViewport(Core::Platform::Generic::Window::CEWindow* window, uint32 width,
			                                   uint32 height,
			                                   CEFormat colorFormat, CEFormat depthFormat) override final;

			virtual CEICommandContext* GetDefaultCommandContext() override final;

			virtual void CheckRayTracingSupport(Main::CERayTracingSupport& outSupport) override final;
			virtual void CheckShadingRateSupport(Main::CEShadingRateSupport& outSupport) override final;

		private:
			template <typename TCEDXTexture>
			TCEDXTexture* CreateTexture(CEFormat format,
			                            uint32 sizeX, uint32 sizeY, uint32 sizeZ,
			                            uint32 numMips,
			                            uint32 numSamplers,
			                            uint32 flags,
			                            CEResourceState initialState,
			                            const CEResourceData* initialData,
			                            const CEClearValue& optimalClearValue);

			template <typename TCEDXBuffer>
			bool FinishBufferResource(TCEDXBuffer* buffer,
			                          uint32 sizeInBytes,
			                          uint32 flags,
			                          CEResourceState initialState,
			                          const CEResourceData* initialData);

			RenderLayer::CEDXDevice* Device;
			Core::Common::CERef<RenderLayer::CEDXCommandContext> DirectCommandContext;
			RenderLayer::CEDXRootSignatureCache* RootSignatureCache;

			RenderLayer::CEDXOfflineDescriptorHeap* ResourceOfflineDescriptorHeap = nullptr;
			RenderLayer::CEDXOfflineDescriptorHeap* RenderTargetOfflineDescriptorHeap = nullptr;
			RenderLayer::CEDXOfflineDescriptorHeap* DepthStencilOfflineDescriptorHeap = nullptr;
			RenderLayer::CEDXOfflineDescriptorHeap* SamplerOfflineDescriptorHeap = nullptr;
		};

	}}
