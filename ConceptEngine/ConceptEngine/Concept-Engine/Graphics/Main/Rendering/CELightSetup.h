#pragma once
#include "../../../Core/Containers/CEArray.h"
#include "../../../Core/Containers/CEStaticArray.h"
#include "../../../Core/Common/CETypes.h"
#include "../../../Core/Common/CERef.h"
#include "../RenderLayer/CEBuffer.h"
#include "../RenderLayer/CETexture.h"
#include "../RenderLayer/CEResourceViews.h"
#include "../RenderLayer/CECommandList.h"
#include "../Common/API.h"
#include "../../../Render/Scene/CEScene.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	struct CEPointLightData {
		CEVectorFloat3 Color = CEVectorFloat3();
		float Padding0;
	};

	struct CEShadowCastingPointLightData {
		CEVectorFloat3 Color = CEVectorFloat3();
		float ShadowBias = 0.005f;
		float FarPlane = 10.0f;
		float MaxShadowBias = 0.05f;
		float Padding0;
		float Padding1;
	};

	struct CEPointLightShadowMapGenerationData {

		Core::Containers::CEStaticArray<CEMatrixFloat4X4, 6> Matrix;
		Core::Containers::CEStaticArray<CEMatrixFloat4X4, 6> ViewMatrix;
		Core::Containers::CEStaticArray<CEMatrixFloat4X4, 6> ProjMatrix;
		CEVectorFloat3 Position;

		float FarPlane;
	};

	struct CEDirectionalLightData {
		CEVectorFloat3 Color = CEVectorFloat3(1.0f, 1.0f, 1.0f);
		CEVectorFloat3 Direction = CEVectorFloat3(0.0f, -1.0f, 0.0f);
		CEMatrixFloat4X4 LightMatrix;
		float ShadowBias = 0.005f;
		float MaxShadowBias = 0.05f;
	};

	struct CEDirLightShadowMapGenerationData {
		CEMatrixFloat4X4 Matrix;
		CEVectorFloat3 Position;
		float FarPlane;

	};

	struct CELightSetup {

		const RenderLayer::CEFormat ShadowMapFormat = RenderLayer::CEFormat::D32_Float;
		const RenderLayer::CEFormat LightProbeFormat = RenderLayer::CEFormat::R16G16B16A16_Float;
		const uint32 MaxPointLights = 256;
		const uint32 MaxDirectionalLights = 256;
		const uint32 MaxPointLightShadows = 8;
		const uint16 ShadowMapWidth = 4096;
		const uint16 ShadowMapHeight = 4096;
		const uint16 PointLightShadowSize = 1024;

		CELightSetup() = default;
		~CELightSetup() = default;

		bool Create();

		void BeginFrame(RenderLayer::CECommandList& commandList, const Render::Scene::CEScene& scene);
		void Release();

		Core::Containers::CEArray<CEPointLightData> PointLightsData;
		Core::Common::CERef<RenderLayer::CEConstantBuffer> PointLightsBuffer;
		Core::Common::CERef<RenderLayer::CEConstantBuffer> PointLightsPosRadBuffer;

		Core::Containers::CEArray<CEPointLightShadowMapGenerationData> PointLightShadowMapsGenerationData;
		Core::Containers::CEArray<CEShadowCastingPointLightData> ShadowCastingPointLightsData;
		Core::Common::CERef<RenderLayer::CEConstantBuffer> ShadowCastingPointLightsBuffer;
		Core::Common::CERef<RenderLayer::CEConstantBuffer> ShadowCastingPointLightsPosRadBuffer;
		Core::Common::CERef<RenderLayer::CETextureCubeArray> PointLightShadowMaps;
		Core::Containers::CEArray<RenderLayer::CEDepthStencilViewCube> PointLightShadowMapDSVs;

		Core::Containers::CEArray<CEDirLightShadowMapGenerationData> DirLightShadowMapGenerationData;
		Core::Containers::CEArray<CEDirectionalLightData> DirectionalLightsData;
		Core::Common::CERef<RenderLayer::CEConstantBuffer> DirectionalLightsBuffer;
		Core::Common::CERef<RenderLayer::CETexture2D> DirLightShadowMaps;

		Core::Common::CERef<RenderLayer::CETextureCube> IrradianceMap;
		Core::Common::CERef<RenderLayer::CEUnorderedAccessView> IrradianceMapUAV;

		Core::Common::CERef<RenderLayer::CETextureCube> SpecularIrradianceMap;
		Core::Containers::CEArray<Core::Common::CERef<RenderLayer::CEUnorderedAccessView>> SpecularIrradianceMapUAVs;
		Core::Containers::CEArray<RenderLayer::CEUnorderedAccessView*> WeakSpecularIrradianceMapUAVs;

		Core::Containers::CEArray<CEVectorFloat4> PointLightsPosRad;
		Core::Containers::CEArray<CEVectorFloat4> ShadowCastingPointLightsPosRad;

	};
}
