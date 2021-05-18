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

#include <DirectXMath.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace ConceptEngine::Graphics::Main::Rendering {
	struct CEPointLightData {
#if DIRECTX_API
		DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3();
#elif VULKAN_API || OPENGL_API
		glm::vec3 Color = glm::vec3();
#else
		DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3();
#endif

		float Padding0;
	};

	struct CEShadowCastingPointLightData {
#if DIRECTX_API
		DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3();
#elif VULKAN_API || OPENGL_API
		glm::vec3 Color = glm::vec3();
#else
		DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3();
#endif

		float ShadowBias = 0.005f;
		float FarPlane = 10.0f;
		float MaxShadowBias = 0.05f;
		float Padding0;
		float Padding1;
	};

	struct CEPointLightShadowMapGenerationData {
#if DIRECTX_API
		Core::Containers::CEStaticArray<DirectX::XMFLOAT4X4, 6> Matrix;
		Core::Containers::CEStaticArray<DirectX::XMFLOAT4X4, 6> ViewMatrix;
		Core::Containers::CEStaticArray<DirectX::XMFLOAT4X4, 6> ProjMatrix;
		DirectX::XMFLOAT3 Position;
#elif VULKAN_API || OPENGL_API
		Core::Containers::CEStaticArray<glm::mat4x4, 6> Matrix;
		Core::Containers::CEStaticArray<glm::mat4x4, 6> ViewMatrix;
		Core::Containers::CEStaticArray<glm::mat4x4, 6> ProjMatrix;
		glm::vec3 Position;
#else
		Core::Containers::CEStaticArray<DirectX::XMFLOAT4X4, 6> Matrix;
		Core::Containers::CEStaticArray<DirectX::XMFLOAT4X4, 6> ViewMatrix;
		Core::Containers::CEStaticArray<DirectX::XMFLOAT4X4, 6> ProjMatrix;
		DirectX::XMFLOAT3 Position;
#endif

		float FarPlane;
	};

	struct CEDirectionalLightData {
#if DIRECTX_API
		DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		DirectX::XMFLOAT3 Direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
		DirectX::XMFLOAT4X4 LightMatrix;
#elif VULKAN_API || OPENGL_API
		glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::mat4x4 LightMatrix;
#else
		DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		DirectX::XMFLOAT3 Direction = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
		DirectX::XMFLOAT4X4 LightMatrix;
#endif

		float ShadowBias = 0.005f;
		float MaxShadowBias = 0.05f;
	};

	struct CEDirLightShadowMapGenerationData {
#if DIRECTX_API
		DirectX::XMFLOAT4X4 Matrix;
		DirectX::XMFLOAT3 Position;
#elif VULKAN_API || OPENGL_API
		glm::mat4x4 Matrix;
		glm::vec3 Position;
#else
		DirectX::XMFLOAT4X4 Matrix;
		DirectX::XMFLOAT3 Position;
#endif
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

#if DIRECTX_API
		Core::Containers::CEArray<DirectX::XMFLOAT4> PointLightsPosRad;
		Core::Containers::CEArray<DirectX::XMFLOAT4> ShadowCastingPointLightsPosRad;
#elif VULKAN_API || OPENGL_API
		Core::Containers::CEArray<glm::vec4> PointLightsPosRad;
				Core::Containers::CEArray<glm::vec4> ShadowCastingPointLightsPosRad;
#else
		Core::Containers::CEArray<DirectX::XMFLOAT4> PointLightsPosRad;
		Core::Containers::CEArray<DirectX::XMFLOAT4> ShadowCastingPointLightsPosRad;
#endif

	};
}
