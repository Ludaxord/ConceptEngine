#pragma once
#include <unordered_map>

#include "CEDebugUI.h"
#include "CEMeshDrawCommand.h"

#include "../RenderLayer/CERendering.h"
#include "../RenderLayer/CETexture.h"
#include "../RenderLayer/CEBuffer.h"
#include "../RenderLayer/CESamplerState.h"
#include "../RenderLayer/CERayTracing.h"
#include "../RenderLayer/CEResourceViews.h"
#include "../RenderLayer/CEViewport.h"

#include "../../../Core/Common/CERef.h"
#include "../../../Core/Containers/CEArray.h"


namespace ConceptEngine::Graphics::Main::Rendering {
	template <typename TResource>
	class CEPtrResourceCache {

	};

	struct CEFrameResources {
		CEFrameResources() = default;
		~CEFrameResources() = default;

		void Release();

		const RenderLayer::CEFormat DepthBufferFormat = RenderLayer::CEFormat::D32_Float;
		const RenderLayer::CEFormat SSAOBufferFormat = RenderLayer::CEFormat::R16_Float;
		const RenderLayer::CEFormat FinalTargetFormat = RenderLayer::CEFormat::R16G16B16A16_Float;
		const RenderLayer::CEFormat RTOutputFormat = RenderLayer::CEFormat::R16G16B16A16_Float;
		const RenderLayer::CEFormat RenderTargetFormat = RenderLayer::CEFormat::R8G8B8A8_Unorm;
		const RenderLayer::CEFormat AlbedoFormat = RenderLayer::CEFormat::R8G8B8A8_Unorm;
		const RenderLayer::CEFormat MaterialFormat = RenderLayer::CEFormat::R8G8B8A8_Unorm;
		const RenderLayer::CEFormat NormalFormat = RenderLayer::CEFormat::R10G10B10A2_Unorm;
		const RenderLayer::CEFormat ViewNormalFormat = RenderLayer::CEFormat::R10G10B10A2_Unorm;


		RenderLayer::CETexture2D* BackBuffer = nullptr;

		Core::Common::CERef<RenderLayer::CEConstantBuffer> CameraBuffer;
		Core::Common::CERef<RenderLayer::CEConstantBuffer> TransformBuffer;

		Core::Common::CERef<RenderLayer::CESamplerState> DirectionalShadowSampler;
		Core::Common::CERef<RenderLayer::CESamplerState> PointShadowSampler;
		Core::Common::CERef<RenderLayer::CESamplerState> IrradianceSampler;

		Core::Common::CERef<RenderLayer::CETextureCube> Skybox;

		Core::Common::CERef<RenderLayer::CETexture2D> IntegrationLUT;
		Core::Common::CERef<RenderLayer::CESamplerState> IntegrationLUTSampler;

		Core::Common::CERef<RenderLayer::CETexture2D> SSAOBuffer;
		Core::Common::CERef<RenderLayer::CETexture2D> FinalTarget;
		Core::Common::CERef<RenderLayer::CETexture2D> GBuffer[5];
		Core::Common::CERef<RenderLayer::CESamplerState> GBufferSampler;
		Core::Common::CERef<RenderLayer::CESamplerState> FXAASampler;

		Core::Common::CERef<RenderLayer::CESamplerState> StdInputLayout;

		Core::Common::CERef<RenderLayer::CETexture2D> RTOutput;
		Core::Common::CERef<RenderLayer::CERayTracingScene> RTScene;

		RenderLayer::CERayTracingShaderResources GlobalResources;
		RenderLayer::CERayTracingShaderResources RayGenLocalResources;
		RenderLayer::CERayTracingShaderResources MissLocalResources;
		Core::Containers::CEArray<RenderLayer::CERayTracingGeometryInstance> RTGeometryInstances;

		Core::Containers::CEArray<RenderLayer::CERayTracingShaderResources> RTHitGroupResources;
		std::unordered_map<class CEMesh*, uint32> RTMeshToHitGroupIndex;
		CEPtrResourceCache<RenderLayer::CEShaderResourceView> RTMaterialTextureCache;

		Core::Containers::CEArray<CEMeshDrawCommand> DeferredVisibleCommands;
		Core::Containers::CEArray<CEMeshDrawCommand> ForwardVisibleCommands;

		Core::Containers::CEArray<ImGuiImage> DebugTextures;

		Core::Common::CERef<RenderLayer::CEViewport> MainWindowViewport;
	};
}
