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
#if WINDOWS_PLATFORM
		XMFLOAT3 Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
#else
		CEVectorFloat3 Color = CEVectorFloat3(1.0f, 1.0f, 1.0f);
#endif
		float Padding0;
	};

	struct CEShadowCastingPointLightData {
#if WINDOWS_PLATFORM
		XMFLOAT3 Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
#else
		CEVectorFloat3 Color = CEVectorFloat3(1.0f, 1.0f, 1.0f);
#endif
		float ShadowBias = 0.005f;
		float FarPlane = 10.0f;
		float MaxShadowBias = 0.05f;
		float Padding0;
		float Padding1;
	};

	struct CEPointLightShadowMapGenerationData {
#if WINDOWS_PLATFORM
		CEStaticArray<XMFLOAT4X4, 6> Matrix;
		CEStaticArray<XMFLOAT4X4, 6> ViewMatrix;
		CEStaticArray<XMFLOAT4X4, 6> ProjMatrix;
		XMFLOAT3 Position;
#else
		CEStaticArray<CEMatrixFloat4X4, 6> Matrix;
		CEStaticArray<CEMatrixFloat4X4, 6> ViewMatrix;
		CEStaticArray<CEMatrixFloat4X4, 6> ProjMatrix;
		CEVectorFloat3 Position;
#endif
		float FarPlane;
	};

	struct CEDirectionalLightData {
#if WINDOWS_PLATFORM
		XMFLOAT3 Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
		XMFLOAT3 Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
		XMFLOAT4X4 LightMatrix;
#else
		CEVectorFloat3 Color = CEVectorFloat3(1.0f, 1.0f, 1.0f);
		CEVectorFloat3 Direction = CEVectorFloat3(0.0f, -1.0f, 0.0f);
		CEMatrixFloat4X4 LightMatrix;
#endif

		float ShadowBias = 0.005f;
		float MaxShadowBias = 0.05f;
	};

	struct CEDirLightShadowMapGenerationData {
#if WINDOWS_PLATFORM
		DirectX::XMFLOAT4X4 Matrix;
		DirectX::XMFLOAT3 Position;
#else
		CEMatrixFloat4X4 Matrix;
		CEVectorFloat3 Position;
#endif
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

#if WINDOWS_PLATFORM
		CEArray<DirectX::XMFLOAT4> PointLightsPosRad;
		CEArray<DirectX::XMFLOAT4> ShadowCastingPointLightsPosRad;
#else
		CEArray<CEVectorFloat4> PointLightsPosRad;
		CEArray<CEVectorFloat4> ShadowCastingPointLightsPosRad;
#endif
	};
}
