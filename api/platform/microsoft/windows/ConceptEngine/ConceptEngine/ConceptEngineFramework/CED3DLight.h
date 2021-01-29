#pragma once
#include <DirectXMath.h>

#include "CELight.h"

namespace Concept::GraphicsEngine::Direct3D::Lighting {
	struct PointLight : CELight {
		PointLight(): PositionWS(0.0f, 0.0f, 0.0f, 0.0f),
		              PositionVS(0.0f, 0.0f, 0.0f, 0.0f),
		              Color(1.0f, 1.0f, 1.0f, 1.0f),
		              ConstantAttenuation(1.0f),
		              LinearAttenuation(0.0f),
		              QuadraticAttenuation(0.0f) {
		}

		/*
		 * Light position in world space
		 */
		DirectX::XMFLOAT4 PositionWS;
		//-------------------------------------------------- (16 byte boundary)
		/*
		 * Light position in view space
		 */
		DirectX::XMFLOAT4 PositionVS;
		//-------------------------------------------------- (16 byte boundary)
		DirectX::XMFLOAT4 Color;
		//-------------------------------------------------- (16 byte boundary)

		float ConstantAttenuation;
		float LinearAttenuation;
		float QuadraticAttenuation;
		// Add padding to align to 16 bytes
		float Padding;
		//-------------------------------------------------- (16 byte boundary)
		// Total:                              16 * 4 = 64 bytes
	};

	struct SpotLight : CELight {
		SpotLight(): PositionWS(0.0f, 0.0f, 0.0f, 1.0f),
		             PositionVS(0.0f, 0.0f, 0.0f, 1.0f),
		             DirectionWS(0.0f, 0.0f, 1.0f, 0.0f),
		             DirectionVS(0.0f, 0.0f, 1.0f, 0.0f),
		             Color(1.0f, 1.0f, 1.0f, 1.0f),
		             SpotAngle(DirectX::XM_PIDIV2),
		             ConstantAttenuation(1.0f),
		             LinearAttenuation(0.0f),
		             QuadraticAttenuation(0.0f) {
		}

		/*
		 * Light position in world space
		 */
		DirectX::XMFLOAT4 PositionWS;
		//-------------------------------------------------- (16 byte boundary)
		/*
		 * Light position in view space
		 */
		DirectX::XMFLOAT4 PositionVS;
		//-------------------------------------------------- (16 byte boundary)
		/*
		 * Light direction in world space
		 */
		DirectX::XMFLOAT4 DirectionWS;
		//-------------------------------------------------- (16 byte boundary)
		/*
		 * Light direction in view space
		 */
		DirectX::XMFLOAT4 DirectionVS;
		//-------------------------------------------------- (16 byte boundary)
		DirectX::XMFLOAT4 Color;
		//-------------------------------------------------- (16 byte boundary)

		float SpotAngle;
		float ConstantAttenuation;
		float LinearAttenuation;
		float QuadraticAttenuation;
		//-------------------------------------------------- (16 byte boundary)
		// Total:                              16 * 4 = 64 bytes
	};
}
