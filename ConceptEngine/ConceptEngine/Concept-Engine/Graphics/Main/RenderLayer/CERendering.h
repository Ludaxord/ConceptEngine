#pragma once
#include "../../../Core/Common/CETypes.h"

#include "../../../Math/CEColor.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {

	enum class CECubeFace {
		PosX = 0,
		NegX = 1,
		PosY = 2,
		NegY = 3,
		PosZ = 4,
		NegZ = 5
	};

	inline uint32 GetCubeFaceIndex(CECubeFace cubeFace) {
		return static_cast<uint32>(cubeFace);
	}

	inline CECubeFace GetCubeFaceFromIndex(uint32 index) {
		if (index > GetCubeFaceIndex(CECubeFace::NegZ)) {
			return static_cast<CECubeFace>(-1);
		}
		else {
			return static_cast<CECubeFace>(index);
		}
	}

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

	inline const char* ToString(CEFormat Format) {
		switch (Format) {
		case CEFormat::R32G32B32A32_Typeless: return "R32G32B32A32_Typeless";
		case CEFormat::R32G32B32A32_Float: return "R32G32B32A32_Float";
		case CEFormat::R32G32B32A32_Uint: return "R32G32B32A32_Uint";
		case CEFormat::R32G32B32A32_Sint: return "R32G32B32A32_Sint";
		case CEFormat::R32G32B32_Typeless: return "R32G32B32_Typeless";
		case CEFormat::R32G32B32_Float: return "R32G32B32_Float";
		case CEFormat::R32G32B32_Uint: return "R32G32B32_Uint";
		case CEFormat::R32G32B32_Sint: return "R32G32B32_Sint";
		case CEFormat::R16G16B16A16_Typeless: return "R16G16B16A16_Typeless";
		case CEFormat::R16G16B16A16_Float: return "R16G16B16A16_Float";
		case CEFormat::R16G16B16A16_Unorm: return "R16G16B16A16_Unorm";
		case CEFormat::R16G16B16A16_Uint: return "R16G16B16A16_Uint";
		case CEFormat::R16G16B16A16_Snorm: return "R16G16B16A16_Snorm";
		case CEFormat::R16G16B16A16_Sint: return "R16G16B16A16_Sint";
		case CEFormat::R32G32_Typeless: return "R32G32_Typeless";
		case CEFormat::R32G32_Float: return "R32G32_Float";
		case CEFormat::R32G32_Uint: return "R32G32_Uint";
		case CEFormat::R32G32_Sint: return "R32G32_Sint";
		case CEFormat::R10G10B10A2_Typeless: return "R10G10B10A2_Typeless";
		case CEFormat::R10G10B10A2_Unorm: return "R10G10B10A2_Unorm";
		case CEFormat::R10G10B10A2_Uint: return "R10G10B10A2_Uint";
		case CEFormat::R11G11B10_Float: return "R11G11B10_Float";
		case CEFormat::R8G8B8A8_Typeless: return "R8G8B8A8_Typeless";
		case CEFormat::R8G8B8A8_Unorm: return "R8G8B8A8_Unorm";
		case CEFormat::R8G8B8A8_Unorm_SRGB: return "R8G8B8A8_Unorm_SRGB";
		case CEFormat::R8G8B8A8_Uint: return "R8G8B8A8_Uint";
		case CEFormat::R8G8B8A8_Snorm: return "R8G8B8A8_Snorm";
		case CEFormat::R8G8B8A8_Sint: return "R8G8B8A8_Sint";
		case CEFormat::R16G16_Typeless: return "R16G16_Typeless";
		case CEFormat::R16G16_Float: return "R16G16_Float";
		case CEFormat::R16G16_Unorm: return "R16G16_Unorm";
		case CEFormat::R16G16_Uint: return "R16G16_Uint";
		case CEFormat::R16G16_Snorm: return "R16G16_Snorm";
		case CEFormat::R16G16_Sint: return "R16G16_Sint";
		case CEFormat::R32_Typeless: return "R32_Typeless";
		case CEFormat::D32_Float: return "D32_Float";
		case CEFormat::R32_Float: return "R32_Float";
		case CEFormat::R32_Uint: return "R32_Uint";
		case CEFormat::R32_Sint: return "R32_Sint";
		case CEFormat::R24G8_Typeless: return "R24G8_Typeless";
		case CEFormat::D24_Unorm_S8_Uint: return "D24_Unorm_S8_Uint";
		case CEFormat::R24_Unorm_X8_Typeless: return "R24_Unorm_X8_Typeless";
		case CEFormat::X24_Typeless_G8_Uint: return "X24_Typeless_G8_Uint";
		case CEFormat::R8G8_Typeless: return "R8G8_Typeless";
		case CEFormat::R8G8_Unorm: return "R8G8_Unorm";
		case CEFormat::R8G8_Uint: return "R8G8_Uint";
		case CEFormat::R8G8_Snorm: return "R8G8_Snorm";
		case CEFormat::R8G8_Sint: return "R8G8_Sint";
		case CEFormat::R16_Typeless: return "R16_Typeless";
		case CEFormat::R16_Float: return "R16_Float";
		case CEFormat::D16_Unorm: return "D16_Unorm";
		case CEFormat::R16_Unorm: return "R16_Unorm";
		case CEFormat::R16_Uint: return "R16_Uint";
		case CEFormat::R16_Snorm: return "R16_Snorm";
		case CEFormat::R16_Sint: return "R16_Sint";
		case CEFormat::R8_Typeless: return "R8_Typeless";
		case CEFormat::R8_Unorm: return "R8_Unorm";
		case CEFormat::R8_Uint: return "R8_Uint";
		case CEFormat::R8_Snorm: return "R8_Snorm";
		case CEFormat::R8_Sint: return "R8_Sint";
		default: return "Unknown";
		}
	}

	inline uint32 GetByteStrideFromFormat(CEFormat format) {
		switch (format) {
		case CEFormat::R32G32B32A32_Float:
		case CEFormat::R32G32B32A32_Sint:
		case CEFormat::R32G32B32A32_Typeless:
		case CEFormat::R32G32B32A32_Uint: {
			return 16;
		}
		case CEFormat::R32G32B32_Typeless:
		case CEFormat::R32G32B32_Float:
		case CEFormat::R32G32B32_Uint:
		case CEFormat::R32G32B32_Sint: {
			return 12;
		}
		case CEFormat::R16G16B16A16_Typeless:
		case CEFormat::R16G16B16A16_Float:
		case CEFormat::R16G16B16A16_Unorm:
		case CEFormat::R16G16B16A16_Uint:
		case CEFormat::R16G16B16A16_Snorm:
		case CEFormat::R16G16B16A16_Sint:
		case CEFormat::R32G32_Typeless:
		case CEFormat::R32G32_Float:
		case CEFormat::R32G32_Uint:
		case CEFormat::R32G32_Sint: {
			return 8;
		}
		case CEFormat::R10G10B10A2_Typeless:
		case CEFormat::R10G10B10A2_Unorm:
		case CEFormat::R10G10B10A2_Uint:
		case CEFormat::R11G11B10_Float:
		case CEFormat::R8G8B8A8_Typeless:
		case CEFormat::R8G8B8A8_Unorm:
		case CEFormat::R8G8B8A8_Unorm_SRGB:
		case CEFormat::R8G8B8A8_Uint:
		case CEFormat::R8G8B8A8_Snorm:
		case CEFormat::R8G8B8A8_Sint:
		case CEFormat::R16G16_Typeless:
		case CEFormat::R16G16_Float:
		case CEFormat::R16G16_Unorm:
		case CEFormat::R16G16_Uint:
		case CEFormat::R16G16_Snorm:
		case CEFormat::R16G16_Sint:
		case CEFormat::R32_Typeless:
		case CEFormat::D32_Float:
		case CEFormat::R32_Float:
		case CEFormat::R32_Uint:
		case CEFormat::R32_Sint:
		case CEFormat::R24G8_Typeless:
		case CEFormat::D24_Unorm_S8_Uint:
		case CEFormat::R24_Unorm_X8_Typeless:
		case CEFormat::X24_Typeless_G8_Uint: {
			return 4;
		}
		case CEFormat::R8G8_Typeless:
		case CEFormat::R8G8_Unorm:
		case CEFormat::R8G8_Uint:
		case CEFormat::R8G8_Snorm:
		case CEFormat::R8G8_Sint:
		case CEFormat::R16_Typeless:
		case CEFormat::R16_Float:
		case CEFormat::D16_Unorm:
		case CEFormat::R16_Unorm:
		case CEFormat::R16_Uint:
		case CEFormat::R16_Snorm:
		case CEFormat::R16_Sint: {
			return 2;
		}
		case CEFormat::R8_Typeless:
		case CEFormat::R8_Unorm:
		case CEFormat::R8_Uint:
		case CEFormat::R8_Snorm:
		case CEFormat::R8_Sint: {
			return 1;
		}
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

	inline const char* ToString(CEPrimitiveTopologyType primitiveTopologyType) {
		switch (primitiveTopologyType) {
		case CEPrimitiveTopologyType::Undefined: return "Undefined";
		case CEPrimitiveTopologyType::Point: return "Point";
		case CEPrimitiveTopologyType::Line: return "Line";
		case CEPrimitiveTopologyType::Triangle: return "Triangle";
		case CEPrimitiveTopologyType::Patch: return "Patch";
		default: return "Unknown";
		}
	}

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

	inline const char* ToString(CEComparisonFunc ComparisonFunc) {
		switch (ComparisonFunc) {
		case CEComparisonFunc::Never: return "Never";
		case CEComparisonFunc::Less: return "Less";
		case CEComparisonFunc::Equal: return "Equal";
		case CEComparisonFunc::LessEqual: return "LessEqual";
		case CEComparisonFunc::Greater: return "Greater";
		case CEComparisonFunc::NotEqual: return "NotEqual";
		case CEComparisonFunc::GreaterEqual: return "GreaterEqual";
		case CEComparisonFunc::Always: return "Always";
		default: return "Unknown";
		}
	}

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

	inline const char* ToString(CEResourceState ResourceState) {
		switch (ResourceState) {
		case CEResourceState::Common: return "Common";
		case CEResourceState::VertexAndConstantBuffer: return "VertexAndConstantBuffer";
		case CEResourceState::IndexBuffer: return "IndexBuffer";
		case CEResourceState::RenderTarget: return "RenderTarget";
		case CEResourceState::UnorderedAccess: return "UnorderedAccess";
		case CEResourceState::DepthWrite: return "DepthWrite";
		case CEResourceState::DepthRead: return "DepthRead";
		case CEResourceState::NonPixelShaderResource: return "NonPixelShaderResource";
		case CEResourceState::PixelShaderResource: return "PixelShaderResource";
		case CEResourceState::CopyDest: return "CopyDest";
		case CEResourceState::CopySource: return "CopySource";
		case CEResourceState::ResolveDest: return "ResolveDest";
		case CEResourceState::ResolveSource: return "ResolveSource";
		case CEResourceState::RayTracingAccelerationStructure: return "RayTracingAccelerationStructure";
		case CEResourceState::ShadingRateSource: return "ShadingRateSource";
		case CEResourceState::Present: return "Present";
		default: return "Unknown";
		}
	}

	enum class CEShadingRate {
		VRS_1x1 = 0x0,
		VRS_1x2 = 0x1,
		VRS_2x1 = 0x4,
		VRS_2x2 = 0x5,
		VRS_2x4 = 0x6,
		VRS_4x2 = 0x9,
		VRS_4x4 = 0xa,
	};

	inline const char* ToString(CEShadingRate ShadingRate) {
		switch (ShadingRate) {
		case CEShadingRate::VRS_1x1: return "VRS_1x1";
		case CEShadingRate::VRS_1x2: return "VRS_1x2";
		case CEShadingRate::VRS_2x1: return "VRS_2x1";
		case CEShadingRate::VRS_2x2: return "VRS_2x2";
		case CEShadingRate::VRS_2x4: return "VRS_2x4";
		case CEShadingRate::VRS_4x2: return "VRS_4x2";
		case CEShadingRate::VRS_4x4: return "VRS_4x4";
		default: return "Unknown";
		}
	}

	enum class CEPrimitiveTopology {
		Undefined = 0,
		PointList = 1,
		LineList = 2,
		LineStrip = 3,
		TriangleList = 4,
		TriangleStrip = 5,
	};

	inline const char* ToString(CEPrimitiveTopology ResourceState) {
		switch (ResourceState) {
		case CEPrimitiveTopology::Undefined: return "Undefined";
		case CEPrimitiveTopology::PointList: return "PointList";
		case CEPrimitiveTopology::LineList: return "LineList";
		case CEPrimitiveTopology::LineStrip: return "LineStrip";
		case CEPrimitiveTopology::TriangleList: return "TriangleList";
		case CEPrimitiveTopology::TriangleStrip: return "TriangleStrip";
		default: return "Unknown";
		}
	}

	struct CEResourceData {
		CEResourceData() : Data(nullptr) {

		}

		CEResourceData(const void* data, uint32 sizeInBytes) : Data(data), SizeInBytes(sizeInBytes) {

		}

		CEResourceData(const void* data, CEFormat format, uint32 width): Data(data), Format(format), Width(width),
		                                                                 Height(0) {

		}

		CEResourceData(const void* data, CEFormat format, uint32 width, uint32 height): Data(data), Format(format),
			Width(width), Height(height) {

		}

		void Set(const void* data, uint32 sizeInBytes) {
			Data = data;
			SizeInBytes = sizeInBytes;
		}

		void Set(const void* data, CEFormat format, uint32 width) {
			Data = data;
			Format = format;
			Width = width;
		}

		void Set(const void* data, CEFormat format, uint32 width, uint32 height) {
			Set(data, format, width);
			Height = height;
		}

		const void* GetData() const {
			return Data;
		}

		uint32 GetSizeInBytes() const {
			return SizeInBytes;
		}

		uint32 GetPitch() const {
			return GetByteStrideFromFormat(Format) * Width;
		}

		uint32 GetSlicePitch() const {
			return GetByteStrideFromFormat(Format) * Width * Height;
		}

	private:
		const void* Data;

		union {
			struct {
				uint32 SizeInBytes;
			};

			struct {
				CEFormat Format;
				uint32 Width;
				uint32 Height;
			};
		};
	};

	struct CEDepthStencilF {
		CEDepthStencilF() = default;

		CEDepthStencilF(float depth, uint8 stencil) : Depth(depth), Stencil(stencil) {

		}

		float Depth = 1.0f;
		uint8 Stencil = 0;
	};

	struct CEClearValue {
		enum class CEType {
			Color = 1,
			DepthStencil = 2
		};

		CEClearValue() : Type(CEType::Color), Format(CEFormat::Unknown), Color(0.0f, 0.0f, 0.0f, 1.0f) {

		}

		CEClearValue(CEFormat format, float depth, uint8 stencil) : Type(CEType::DepthStencil), Format(format),
		                                                            DepthStencil(depth, stencil) {

		}

		CEClearValue(CEFormat format, float r, float g, float b, float a) : Type(CEType::Color), Format(format),
		                                                                    Color(r, g, b, a) {

		}

		CEClearValue(const CEClearValue& o) : Type(o.Type), Format(o.Format), Color() {
			if (o.Type == CEType::Color) {
				Color = o.Color;
			}
			else if (o.Type == CEType::DepthStencil) {
				DepthStencil = o.DepthStencil;
			}
		}

		CEClearValue& operator=(const CEClearValue& o) {
			Type = o.Type;
			Format = o.Format;
			if (o.Type == CEType::Color) {
				Color = o.Color;
			}
			else if (o.Type == CEType::DepthStencil) {
				DepthStencil = o.DepthStencil;
			}

			return *this;
		}

		CEType GetType() const {
			return Type;
		}

		CEFormat GetFormat() const {
			return Format;
		}

		Math::CEColorF& AsColor() {
			Assert(Type == CEType::Color);
			return Color;
		}

		const Math::CEColorF& AsColor() const {
			Assert(Type == CEType::Color);
			return Color;
		}

		CEDepthStencilF& AsDepthStencil() {
			Assert(Type == CEType::DepthStencil);
			return DepthStencil;
		}

		const CEDepthStencilF& AsDepthStencil() const {
			Assert(Type == CEType::DepthStencil);
			return DepthStencil;
		}

	private:
		CEType Type;
		CEFormat Format;

		union {
			Math::CEColorF Color;
			CEDepthStencilF DepthStencil;
		};
	};

	struct CECopyBufferInfo {
		CECopyBufferInfo() = default;

		CECopyBufferInfo(uint64 sourceOffset, uint32 destinationOffset, uint32 sizeInBytes) {

		}

		uint64 SourceOffset = 0;
		uint32 DestinationOffset = 0;
		uint32 SizeInBytes = 0;
	};

	struct CECopyTextureSubresourceInfo {
		CECopyTextureSubresourceInfo() = default;

		CECopyTextureSubresourceInfo(uint32 x, uint32 y, uint32 z, uint32 subresourceIndex) : x(x), y(y), z(z),
			subresourceIndex(subresourceIndex) {

		}

		uint32 x = 0;
		uint32 y = 0;
		uint32 z = 0;
		uint32 subresourceIndex = 0;
	};

	struct CECopyTextureInfo {
		CECopyTextureSubresourceInfo Source;
		CECopyTextureSubresourceInfo Destination;
		uint32 Width = 0;
		uint32 Height = 0;
		uint32 Depth = 0;
	};
}
