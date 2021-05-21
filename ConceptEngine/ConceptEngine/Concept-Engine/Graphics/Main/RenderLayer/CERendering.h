#pragma once
#include "../../../Core/Common/CETypes.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {

	enum class CEFormat {
		Unknown = 0,
		R32G32B32A32_Typeless = 1,
		R32G32B32A32_Float = 2,
		R32G32B32A32_Uint = 3,
		R32G32B32A32_Sint = 4,
		R32G32B32_Typeless = 5,
		R32G32B32_Float = 6,
		R32G32B32_Uint = 7,
		R32G32B32_Sint = 8,
		R16G16B16A16_Typeless = 9,
		R16G16B16A16_Float = 10,
		R16G16B16A16_Unorm = 11,
		R16G16B16A16_Uint = 12,
		R16G16B16A16_Snorm = 13,
		R16G16B16A16_Sint = 14,
		R32G32_Typeless = 15,
		R32G32_Float = 16,
		R32G32_Uint = 17,
		R32G32_Sint = 18,
		R10G10B10A2_Typeless = 23,
		R10G10B10A2_Unorm = 24,
		R10G10B10A2_Uint = 25,
		R11G11B10_Float = 26,
		R8G8B8A8_Typeless = 27,
		R8G8B8A8_Unorm = 28,
		R8G8B8A8_Unorm_SRGB = 29,
		R8G8B8A8_Uint = 30,
		R8G8B8A8_Snorm = 31,
		R8G8B8A8_Sint = 32,
		R16G16_Typeless = 33,
		R16G16_Float = 34,
		R16G16_Unorm = 35,
		R16G16_Uint = 36,
		R16G16_Snorm = 37,
		R16G16_Sint = 38,
		R32_Typeless = 39,
		D32_Float = 40,
		R32_Float = 41,
		R32_Uint = 42,
		R32_Sint = 43,
		R24G8_Typeless = 44,
		D24_Unorm_S8_Uint = 45,
		R24_Unorm_X8_Typeless = 46,
		X24_Typeless_G8_Uint = 47,
		R8G8_Typeless = 48,
		R8G8_Unorm = 49,
		R8G8_Uint = 50,
		R8G8_Snorm = 51,
		R8G8_Sint = 52,
		R16_Typeless = 53,
		R16_Float = 54,
		D16_Unorm = 55,
		R16_Unorm = 56,
		R16_Uint = 57,
		R16_Snorm = 58,
		R16_Sint = 59,
		R8_Typeless = 60,
		R8_Unorm = 61,
		R8_Uint = 62,
		R8_Snorm = 63,
		R8_Sint = 64,
	};

	inline uint32 GetByteStrideFromFormat(CEFormat format) {
		switch (format) {
		default: {
			return 0;
		}
		}
	}

	enum class CEPrimitiveTopologyType {
		Undefined = 0,
		Point = 1,
		Line = 2,
		Triangle = 3,
		Patch = 4
	};

	enum class CEComparisonFunc {
		Never = 1,
		Less = 2,
		Equal = 3,
		LessEqual = 4,
		Greater = 5,
		NotEqual = 6,
		GreaterEqual = 7,
		Always = 8
	};

	enum class CEResourceState {
		Common = 0,
		VertexAndConstantBuffer = 1,
		IndexBuffer = 2,
		RenderTarget = 3,
		UnorderedAccess = 4,
		DepthWrite = 5,
		DepthRead = 6,
		NonPixelShaderResource = 7,
		PixelShaderResource = 8,
		CopyDest = 9,
		CopySource = 10,
		ResolveDest = 11,
		ResolveSource = 12,
		RayTracingAccelerationStructure = 13,
		ShadingRateSource = 14,
		Present = 15,
		GenericRead = 16,
	};

	enum class CEShadingRate {
		    VRS_1x1 = 0x0,
    VRS_1x2 = 0x1,
    VRS_2x1 = 0x4,
    VRS_2x2 = 0x5,
    VRS_2x4 = 0x6,
    VRS_4x2 = 0x9,
    VRS_4x4 = 0xa,
	};

	enum class CEPrimitiveTopology {
		Undefined = 0,
		PointList = 1,
		LineList = 2,
		LineStrip = 3,
		TriangleList = 4,
		TriangleStrip = 5,
	};

	struct CEResourceData {

	};

	struct CEDepthStencilF {

	};

	struct CEClearValue {

	};

	struct CECopyBufferInfo {

	};

	struct CECopyTextureInfo {

	};
}
