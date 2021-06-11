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
		CEVectorFloat3 Color = CEVectorFloat3(1.0f, 1.0f, 1.0f);
		float Padding0;
	};

	struct CEShadowCastingPointLightData {
		CEVectorFloat3 Color = CEVectorFloat3(1.0f, 1.0f, 1.0f);
		float ShadowBias = 0.005f;
		float FarPlane = 10.0f;
		float MaxShadowBias = 0.05f;
		float Padding0;
		float Padding1;
	};

	struct CEPointLightShadowMapGenerationData {

		CEStaticArray<CEMatrixFloat4X4, 6> Matrix;
		CEStaticArray<CEMatrixFloat4X4, 6> ViewMatrix;
		CEStaticArray<CEMatrixFloat4X4, 6> ProjMatrix;
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
	public:
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

		CEArray<CEPointLightData> PointLightsData;
		CERef<RenderLayer::CEConstantBuffer> PointLightsBuffer;
		CERef<RenderLayer::CEConstantBuffer> PointLightsPosRadBuffer;

		CEArray<CEPointLightShadowMapGenerationData> PointLightShadowMapsGenerationData;
		CEArray<CEShadowCastingPointLightData> ShadowCastingPointLightsData;
		CERef<RenderLayer::CEConstantBuffer> ShadowCastingPointLightsBuffer;
		CERef<RenderLayer::CEConstantBuffer> ShadowCastingPointLightsPosRadBuffer;
		CERef<RenderLayer::CETextureCubeArray> PointLightShadowMaps;
		CEArray<RenderLayer::CEDepthStencilViewCube> PointLightShadowMapDSVs;

		CEArray<CEDirLightShadowMapGenerationData> DirLightShadowMapGenerationData;
		CEArray<CEDirectionalLightData> DirectionalLightsData;
		CERef<RenderLayer::CEConstantBuffer> DirectionalLightsBuffer;
		CERef<RenderLayer::CETexture2D> DirLightShadowMaps;

		CERef<RenderLayer::CETextureCube> IrradianceMap;
		CERef<RenderLayer::CEUnorderedAccessView> IrradianceMapUAV;

		CERef<RenderLayer::CETextureCube> SpecularIrradianceMap;
		CEArray<CERef<RenderLayer::CEUnorderedAccessView>> SpecularIrradianceMapUAVs;
		CEArray<RenderLayer::CEUnorderedAccessView*> WeakSpecularIrradianceMapUAVs;

		CEArray<CEVectorFloat4> PointLightsPosRad;
		CEArray<CEVectorFloat4> ShadowCastingPointLightsPosRad;

	};
}
