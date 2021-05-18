#pragma once
#include "CERendering.h"
#include "CEResource.h"
#include "../../../Math/CEColor.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	enum class CESamplerMode : uint8 {
		Unknown = 0,
		Wrap = 1,
		Mirror = 2,
		Clamp = 3,
		Border = 4,
		MirrorOnce = 5,
	};

	enum class CESamplerFilter : uint8 {
		Unknown = 0,
		MinMagMipPoint = 1,
		MinMagPoint_MipLinear = 2,
		MinPoint_MagLinear_MipPoint = 3,
		MinPoint_MagMipLinear = 4,
		MinLinear_MagMipPoint = 5,
		MinLinear_MagPoint_MipLinear = 6,
		MinMagLinear_MipPoint = 7,
		MinMagMipLinear = 8,
		Anistrotopic = 9,
		Comparison_MinMagMipPoint = 10,
		Comparison_MinMagPoint_MipLinear = 11,
		Comparison_MinPoint_MagLinear_MipPoint = 12,
		Comparison_MinPoint_MagMipLinear = 13,
		Comparison_MinLinear_MagMipPoint = 14,
		Comparison_MinLinear_MagPoint_MipLinear = 15,
		Comparison_MinMagLinear_MipPoint = 16,
		Comparison_MinMagMipLinear = 17,
		Comparison_Anistrotopic = 18,
	};

	struct CESamplerStateCreateInfo {
		CESamplerMode AddressU = CESamplerMode::Clamp;
		CESamplerMode AddressV = CESamplerMode::Clamp;
		CESamplerMode AddressW = CESamplerMode::Clamp;
		CESamplerFilter Filter = CESamplerFilter::MinMagMipLinear;
		CEComparisonFunc ComparisonFunc = CEComparisonFunc::Never;
		float MipLOADBias = 0.0f;
		uint32 MaxAnisotropy = 1;
		Math::CEColorF BorderColor;
		float MinLOD = -FLT_MAX;
		float MaxLOD = FLT_MAX;
	};

	class CESamplerState : public CEResource {

	};
}
