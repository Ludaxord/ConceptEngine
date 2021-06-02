#pragma once

#include "../../Main/Managers/CEGraphicsManager.h"
#include "../RenderLayer/CEDXDescriptorHeap.h"
#include "../RenderLayer/CEDXDevice.h"
#include "../RenderLayer/CEDXCommandContext.h"
#include "../RenderLayer/CEDXRootSignature.h"

namespace ConceptEngine::Graphics::DirectX12::Managers {

	template <typename TD3D12Texture>
	D3D12_RESOURCE_DIMENSION GetD3D12TextureResourceDimension();

	template <typename TD3D12Texture>
	bool IsTextureCube();

	class CEDXManager final : public Main::Managers::CEGraphicsManager {
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


		virtual RenderLayer::CETexture2D* CreateTexture2D(RenderLayer::CEFormat format, uint32 width, uint32 height,
		                                                  uint32 numMips,
		                                                  uint32 numSamples, uint32 flags,
		                                                  RenderLayer::CEResourceState initialState,
		                                                  const RenderLayer::CEResourceData* initialData,
		                                                  const RenderLayer::CEClearValue&
		                                                  optimizedClearValue) override final;

		virtual RenderLayer::CETexture2DArray* CreateTexture2DArray(RenderLayer::CEFormat format, uint32 width,
		                                                            uint32 height, uint32 numMips,
		                                                            uint32 numSamples, uint32 numArraySlices,
		                                                            uint32 flags,
		                                                            RenderLayer::CEResourceState initialState,
		                                                            const RenderLayer::CEResourceData* initialData,
		                                                            const RenderLayer::CEClearValue&
		                                                            optimizedClearValue) override final;

		virtual RenderLayer::CETextureCube* CreateTextureCube(RenderLayer::CEFormat format,
		                                                      uint32 size,
		                                                      uint32 numMips,
		                                                      uint32 flags, RenderLayer::CEResourceState initialState,
		                                                      const RenderLayer::CEResourceData* initialData,
		                                                      const RenderLayer::CEClearValue&
		                                                      optimizedClearValue) override final;

		virtual RenderLayer::CETextureCubeArray* CreateTextureCubeArray(RenderLayer::CEFormat format,
		                                                                uint32 size,
		                                                                uint32 numMips,
		                                                                uint32 numArraySlices,
		                                                                uint32 flags,
		                                                                RenderLayer::CEResourceState initialState,
		                                                                const RenderLayer::CEResourceData* initialData,
		                                                                const RenderLayer::CEClearValue&
		                                                                optimalClearValue) override final;

		virtual RenderLayer::CETexture3D* CreateTexture3D(RenderLayer::CEFormat format, uint32 width, uint32 height,
		                                                  uint32 depth,
		                                                  uint32 numMips,
		                                                  uint32 flags, RenderLayer::CEResourceState initialState,
		                                                  const RenderLayer::CEResourceData* initialData,
		                                                  const RenderLayer::CEClearValue&
		                                                  optimizedClearValue) override final;

		virtual RenderLayer::CESamplerState*
		CreateSamplerState(const RenderLayer::CESamplerStateCreateInfo& createInfo) override final;

		virtual RenderLayer::CEVertexBuffer* CreateVertexBuffer(uint32 stride, uint32 numVertices, uint32 flags,
		                                                        RenderLayer::CEResourceState initialState,
		                                                        const RenderLayer::CEResourceData*
		                                                        initialData) override final;

		virtual RenderLayer::CEIndexBuffer* CreateIndexBuffer(RenderLayer::CEIndexFormat format, uint32 numIndices,
		                                                      uint32 flags,
		                                                      RenderLayer::CEResourceState initialState,
		                                                      const RenderLayer::CEResourceData*
		                                                      initialData) override final;

		virtual RenderLayer::CEConstantBuffer* CreateConstantBuffer(uint32 size, uint32 flags,
		                                                            RenderLayer::CEResourceState initialState,
		                                                            const RenderLayer::CEResourceData*
		                                                            initialData) override final;

		virtual RenderLayer::CEStructuredBuffer* CreateStructuredBuffer(uint32 stride, uint32 numElements, uint32 flags,
		                                                                RenderLayer::CEResourceState initialState,
		                                                                const RenderLayer::CEResourceData*
		                                                                initialData) override final;

		virtual RenderLayer::CERayTracingScene* CreateRayTracingScene(uint32 flags,
		                                                              RenderLayer::CERayTracingGeometryInstance*
		                                                              instances,
		                                                              uint32 numInstances) override final;

		virtual RenderLayer::CERayTracingGeometry* CreateRayTracingGeometry(
			uint32 flags, RenderLayer::CEVertexBuffer* vertexBuffer,
			RenderLayer::CEIndexBuffer* indexBuffer) override final;


		virtual RenderLayer::CEShaderResourceView*
		CreateShaderResourceView(const RenderLayer::CEShaderResourceViewCreateInfo& createInfo) override final;

		virtual RenderLayer::CEUnorderedAccessView*
		CreateUnorderedAccessView(const RenderLayer::CEUnorderedAccessViewCreateInfo& createInfo) override final;

		virtual RenderLayer::CERenderTargetView*
		CreateRenderTargetView(const RenderLayer::CERenderTargetViewCreateInfo& createInfo) override final;

		virtual RenderLayer::CEDepthStencilView*
		CreateDepthStencilView(const RenderLayer::CEDepthStencilViewCreateInfo& createInfo) override final;

		virtual RenderLayer::CEComputeShader*
		CreateComputeShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
		virtual RenderLayer::CEVertexShader*
		CreateVertexShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
		virtual RenderLayer::CEHullShader*
		CreateHullShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
		virtual RenderLayer::CEDomainShader*
		CreateDomainShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
		virtual RenderLayer::CEGeometryShader*
		CreateGeometryShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
		virtual RenderLayer::CEMeshShader*
		CreateMeshShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;

		virtual RenderLayer::CEAmplificationShader*
		CreateAmplificationShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
		virtual RenderLayer::CEPixelShader*
		CreatePixelShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;

		virtual RenderLayer::CERayGenShader*
		CreateRayGenShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
		virtual RenderLayer::CERayAnyHitShader*
		CreateRayAnyHitShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
		virtual RenderLayer::CERayClosestHitShader*
		CreateRayClosestHitShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;
		virtual RenderLayer::CERayMissShader*
		CreateRayMissShader(const Core::Containers::CEArray<uint8>& shaderCode) override final;

		virtual RenderLayer::CEDepthStencilState*
		CreateDepthStencilState(const RenderLayer::CEDepthStencilStateCreateInfo& createInfo) override final;
		virtual RenderLayer::CERasterizerState*
		CreateRasterizerState(const RenderLayer::CERasterizerStateCreateInfo& createInfo) override final;
		virtual RenderLayer::CEBlendState*
		CreateBlendState(const RenderLayer::CEBlendStateCreateInfo& createInfo) override final;

		virtual RenderLayer::CEInputLayoutState*
		CreateInputLayout(const RenderLayer::CEInputLayoutStateCreateInfo& createInfo) override final;

		virtual RenderLayer::CEGraphicsPipelineState*
		CreateGraphicsPipelineState(const RenderLayer::CEGraphicsPipelineStateCreateInfo& createInfo) override final;

		virtual RenderLayer::CEComputePipelineState*
		CreateComputePipelineState(const RenderLayer::CEComputePipelineStateCreateInfo& createInfo) override final;

		virtual RenderLayer::CERayTracingPipelineState* CreatRayTracingPipelineState(
			const RenderLayer::CERayTracingPipelineStateCreateInfo& createInfo) override final;

		virtual RenderLayer::CEGPUProfiler* CreateProfiler() override final;
		virtual RenderLayer::CEViewport* CreateViewport(Core::Platform::Generic::Window::CEWindow* window, uint32 width,
		                                                uint32 height,
		                                                RenderLayer::CEFormat colorFormat,
		                                                RenderLayer::CEFormat depthFormat) override final;

		virtual RenderLayer::CEICommandContext* GetDefaultCommandContext() override final;
		virtual bool UAVSupportsFormat(RenderLayer::CEFormat format) override final;

		virtual void CheckRayTracingSupport(Main::CERayTracingSupport& outSupport) override final;
		virtual void CheckShadingRateSupport(Main::CEShadingRateSupport& outSupport) override final;

		virtual std::string GetAdapterName() override final {
			return Device->GetAdapterName();
		};

	private:
		template <typename TCEDXTexture>
		TCEDXTexture* CreateTexture(RenderLayer::CEFormat format,
		                            uint32 sizeX, uint32 sizeY, uint32 sizeZ,
		                            uint32 numMips,
		                            uint32 numSamplers,
		                            uint32 flags,
		                            RenderLayer::CEResourceState initialState,
		                            const RenderLayer::CEResourceData* initialData,
		                            const RenderLayer::CEClearValue& optimalClearValue);

		template <typename TCEDXBuffer>
		bool FinishBufferResource(TCEDXBuffer* buffer,
		                          uint32 sizeInBytes,
		                          uint32 flags,
		                          RenderLayer::CEResourceState initialState,
		                          const RenderLayer::CEResourceData* initialData);

	private:
		RenderLayer::CEDXDevice* Device;
		Core::Common::CERef<RenderLayer::CEDXCommandContext> DirectCommandContext;
		RenderLayer::CEDXRootSignatureCache* RootSignatureCache;

		RenderLayer::CEDXOfflineDescriptorHeap* ResourceOfflineDescriptorHeap = nullptr;
		RenderLayer::CEDXOfflineDescriptorHeap* RenderTargetOfflineDescriptorHeap = nullptr;
		RenderLayer::CEDXOfflineDescriptorHeap* DepthStencilOfflineDescriptorHeap = nullptr;
		RenderLayer::CEDXOfflineDescriptorHeap* SamplerOfflineDescriptorHeap = nullptr;
	};

}
