#pragma once
#include <DirectXMath.h>
#include <glm/vec4.hpp>

namespace ConceptEngine::Graphics::Main::Rendering {
	struct CEPointLightData {
		DirectX::XMFLOAT3 Color = DirectX::XMFLOAT3();
	};


	struct CELightSetup {


#if defined(DIRECTX_API)
		Core::Containers::CEArray<DirectX::XMFLOAT4> PointLightsPosRad;
		Core::Containers::CEArray<DirectX::XMFLOAT4> ShadowCastingPointLightsPosRad;
#elif defined(VULKAN_API) || defined(OPENGL_API)
		Core::Containers::CEArray<glm::vec4> PointLightsPosRad;
				Core::Containers::CEArray<glm::vec4> ShadowCastingPointLightsPosRad;
#else
		Core::Containers::CEArray<DirectX::XMFLOAT4> PointLightsPosRad;
		Core::Containers::CEArray<DirectX::XMFLOAT4> ShadowCastingPointLightsPosRad;
#endif

	};
}
