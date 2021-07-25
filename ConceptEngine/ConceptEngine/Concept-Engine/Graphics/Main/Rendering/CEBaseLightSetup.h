#pragma once
#include <DirectXMath.h>
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

using namespace DirectX;

namespace ConceptEngine::Graphics::Main::Rendering {

	struct PointLightData {
		XMFLOAT3 Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		float Padding0;
	};

	struct ShadowCastingPointLightData {
		XMFLOAT3 Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		float ShadowBias = 0.005f;
		float FarPlane = 10.0f;
		float MaxShadowBias = 0.05f;
		float Padding0;
		float Padding1;
	};

	struct PointLightShadowMapGenerationData {
		CEStaticArray<XMFLOAT4X4, 6> Matrix;
		CEStaticArray<XMFLOAT4X4, 6> ViewMatrix;
		CEStaticArray<XMFLOAT4X4, 6> ProjMatrix;
		float FarPlane;
		XMFLOAT3 Position;
	};

	struct DirectionalLightData {
		XMFLOAT3 Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		float ShadowBias = 0.005f;
		XMFLOAT3 Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
		float MaxShadowBias = 0.05f;
		XMFLOAT4X4 LightMatrix;
	};

	struct DirLightShadowMapGenerationData {
		XMFLOAT4X4 Matrix;
		float FarPlane;
		XMFLOAT3 Position;
	};

	struct CEBaseLightSetup {
    const RenderLayer::CEFormat ShadowMapFormat      = RenderLayer::CEFormat::D32_Float;
    const RenderLayer::CEFormat LightProbeFormat     = RenderLayer::CEFormat::R16G16B16A16_Float;
    const uint32  MaxPointLights       = 256;
    const uint32  MaxDirectionalLights = 256;
    const uint32  MaxPointLightShadows = 8;
    const uint16  ShadowMapWidth       = 4096;
    const uint16  ShadowMapHeight      = 4096;
    const uint16  PointLightShadowSize = 1024;

		CEBaseLightSetup() = default;
		~CEBaseLightSetup() = default;

		bool Create();

		void BeginFrame(RenderLayer::CECommandList& CmdList, const Render::Scene::CEScene& Scene);
		void Release();

		CEArray<XMFLOAT4> PointLightsPosRad;
		CEArray<PointLightData> PointLightsData;
		CERef<RenderLayer::CEConstantBuffer> PointLightsBuffer;
		CERef<RenderLayer::CEConstantBuffer> PointLightsPosRadBuffer;

		CEArray<PointLightShadowMapGenerationData> PointLightShadowMapsGenerationData;
		CEArray<XMFLOAT4> ShadowCastingPointLightsPosRad;
		CEArray<ShadowCastingPointLightData> ShadowCastingPointLightsData;
		CERef<RenderLayer::CEConstantBuffer> ShadowCastingPointLightsBuffer;
		CERef<RenderLayer::CEConstantBuffer> ShadowCastingPointLightsPosRadBuffer;
		CERef<RenderLayer::CETextureCubeArray> PointLightShadowMaps;
		CEArray<RenderLayer::CEDepthStencilViewCube> PointLightShadowMapDSVs;

		CEArray<DirLightShadowMapGenerationData> DirLightShadowMapsGenerationData;
		CEArray<DirectionalLightData> DirectionalLightsData;
		CERef<RenderLayer::CEConstantBuffer> DirectionalLightsBuffer;
		CERef<RenderLayer::CETexture2D> DirLightShadowMaps;

		CERef<RenderLayer::CETextureCube> IrradianceMap;
		CERef<RenderLayer::CEUnorderedAccessView> IrradianceMapUAV;

		CERef<RenderLayer::CETextureCube> SpecularIrradianceMap;
		CEArray<CERef<RenderLayer::CEUnorderedAccessView>> SpecularIrradianceMapUAVs;
		CEArray<RenderLayer::CEUnorderedAccessView*> WeakSpecularIrradianceMapUAVs;
	};
}
