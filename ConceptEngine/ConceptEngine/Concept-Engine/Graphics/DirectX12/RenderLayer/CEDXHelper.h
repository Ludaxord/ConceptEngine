#pragma once

#include "../../../Memory/CEMemory.h"

#include "../../Main/RenderLayer/CEBuffer.h"
#include "../../Main/RenderLayer/CETexture.h"
#include "../../Main/RenderLayer/CEPipelineState.h"
#include "../../Main/RenderLayer/CESamplerState.h"
#include "../../Main/RenderLayer/CERayTracing.h"

#include <dxgi1_2.h>
#include <dxcapi.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <dxgidebug.h>
#include <pix3.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "WinPixEventRuntime.lib")


#define D3D12_MAX_ROOT_PARAMETERS              (64)
#define D3D12_MAX_SHADER_PARAMETERS            (16)
#define D3D12_MAX_HIT_GROUPS                   (512)
#define D3D12_MAX_DESCRIPTOR_RANGES            (64)
#define D3D12_MAX_DESCRIPTOR_RANGE_SIZE        (256)
#define D3D12_MAX_32BIT_SHADER_CONSTANTS_COUNT (32)
#define D3D12_MAX_VERTEX_BUFFER_SLOTS          (8)
#define D3D12_MAX_RENDER_TARGET_COUNT          (8)
#define D3D12_MAX_ONLINE_DESCRIPTOR_COUNT      (2048) // See spec.
#define D3D12_MAX_OFFLINE_DESCRIPTOR_COUNT     (2048) // See spec.

#define D3D12_DEFAULT_QUERY_COUNT (64)

#define D3D12_DEFAULT_ONLINE_RESOURCE_DESCRIPTOR_COUNT (D3D12_MAX_ONLINE_DESCRIPTOR_COUNT)
#define D3D12_DEFAULT_ONLINE_SAMPLER_DESCRIPTOR_COUNT  (D3D12_MAX_ONLINE_DESCRIPTOR_COUNT)

#define D3D12_DEFAULT_DESCRIPTOR_TABLE_HANDLE_COUNT     (16)
#define D3D12_DEFAULT_CONSTANT_BUFFER_COUNT             (16)
#define D3D12_DEFAULT_SHADER_RESOURCE_VIEW_COUNT        (16)
#define D3D12_DEFAULT_UNORDERED_ACCESS_VIEW_COUNT       (16)
#define D3D12_DEFAULT_SAMPLER_STATE_COUNT               (16)

#define D3D12_DEFAULT_LOCAL_CONSTANT_BUFFER_COUNT       (4)
#define D3D12_DEFAULT_LOCAL_SHADER_RESOURCE_VIEW_COUNT  (8)
#define D3D12_DEFAULT_LOCAL_UNORDERED_ACCESS_VIEW_COUNT (4)
#define D3D12_DEFAULT_LOCAL_SAMPLER_STATE_COUNT         (4)

// Space: | Usage:
// 0      | Standard
// 1      | Constants
// 2      | RT Local

#define D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS (1)
#define D3D12_SHADER_REGISTER_SPACE_RT_LOCAL        (2)

namespace ConceptEngine::Graphics::DirectX12::RenderLayer {

	using namespace Main::RenderLayer;

	inline D3D12_HEAP_PROPERTIES GetUploadHeapProperties() {
		D3D12_HEAP_PROPERTIES heapProperties;
		Memory::CEMemory::Memzero(&heapProperties);

		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.VisibleNodeMask = 1;
		heapProperties.CreationNodeMask = 1;

		return heapProperties;
	}

	inline D3D12_HEAP_PROPERTIES GetDefaultHeapProperties() {
		D3D12_HEAP_PROPERTIES HeapProperties;
		Memory::CEMemory::Memzero(&HeapProperties);

		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProperties.VisibleNodeMask = 1;
		HeapProperties.CreationNodeMask = 1;

		return HeapProperties;
	}

	inline D3D12_RESOURCE_FLAGS ConvertBufferFlags(uint32 Flag) {
		D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;
		if (Flag & Main::RenderLayer::CEBufferFlags::BufferFlag_UAV) {
			result |= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return result;
	}

	inline D3D12_RESOURCE_FLAGS ConvertTextureFlags(uint32 Flag) {
		D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;
		if (Flag & Main::RenderLayer::CETextureFlags::TextureFlag_UAV) {
			result |= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
		if (Flag & Main::RenderLayer::CETextureFlags::TextureFlag_RTV) {
			result |= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}
		if (Flag & Main::RenderLayer::CETextureFlags::TextureFlag_DSV) {
			result |= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			if (!(Flag & Main::RenderLayer::CETextureFlags::TextureFlag_SRV)) {
				result |= D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
			}
		}

		return result;
	}

	inline DXGI_FORMAT ConvertFormat(CEFormat Format) {
		switch (Format) {
		case CEFormat::R32G32B32A32_Typeless: return DXGI_FORMAT_R32G32B32A32_TYPELESS;
		case CEFormat::R32G32B32A32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case CEFormat::R32G32B32A32_Uint: return DXGI_FORMAT_R32G32B32A32_UINT;
		case CEFormat::R32G32B32A32_Sint: return DXGI_FORMAT_R32G32B32A32_SINT;
		case CEFormat::R32G32B32_Typeless: return DXGI_FORMAT_R32G32B32_TYPELESS;
		case CEFormat::R32G32B32_Float: return DXGI_FORMAT_R32G32B32_FLOAT;
		case CEFormat::R32G32B32_Uint: return DXGI_FORMAT_R32G32B32_UINT;
		case CEFormat::R32G32B32_Sint: return DXGI_FORMAT_R32G32B32_SINT;
		case CEFormat::R16G16B16A16_Typeless: return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		case CEFormat::R16G16B16A16_Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case CEFormat::R16G16B16A16_Unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case CEFormat::R16G16B16A16_Uint: return DXGI_FORMAT_R16G16B16A16_UINT;
		case CEFormat::R16G16B16A16_Snorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
		case CEFormat::R16G16B16A16_Sint: return DXGI_FORMAT_R16G16B16A16_SINT;
		case Main::RenderLayer::CEFormat::R32G32_Typeless: return DXGI_FORMAT_R32G32_TYPELESS;
		case Main::RenderLayer::CEFormat::R32G32_Float: return DXGI_FORMAT_R32G32_FLOAT;
		case Main::RenderLayer::CEFormat::R32G32_Uint: return DXGI_FORMAT_R32G32_UINT;
		case Main::RenderLayer::CEFormat::R32G32_Sint: return DXGI_FORMAT_R32G32_SINT;
		case Main::RenderLayer::CEFormat::R10G10B10A2_Typeless: return DXGI_FORMAT_R10G10B10A2_TYPELESS;
		case Main::RenderLayer::CEFormat::R10G10B10A2_Unorm: return DXGI_FORMAT_R10G10B10A2_UNORM;
		case Main::RenderLayer::CEFormat::R10G10B10A2_Uint: return DXGI_FORMAT_R10G10B10A2_UINT;
		case Main::RenderLayer::CEFormat::R11G11B10_Float: return DXGI_FORMAT_R11G11B10_FLOAT;
		case Main::RenderLayer::CEFormat::R8G8B8A8_Typeless: return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case Main::RenderLayer::CEFormat::R8G8B8A8_Unorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Main::RenderLayer::CEFormat::R8G8B8A8_Unorm_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case Main::RenderLayer::CEFormat::R8G8B8A8_Uint: return DXGI_FORMAT_R8G8B8A8_UINT;
		case Main::RenderLayer::CEFormat::R8G8B8A8_Snorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
		case Main::RenderLayer::CEFormat::R8G8B8A8_Sint: return DXGI_FORMAT_R8G8B8A8_SINT;
		case Main::RenderLayer::CEFormat::R16G16_Typeless: return DXGI_FORMAT_R16G16_TYPELESS;
		case Main::RenderLayer::CEFormat::R16G16_Float: return DXGI_FORMAT_R16G16_FLOAT;
		case Main::RenderLayer::CEFormat::R16G16_Unorm: return DXGI_FORMAT_R16G16_UNORM;
		case Main::RenderLayer::CEFormat::R16G16_Uint: return DXGI_FORMAT_R16G16_UINT;
		case Main::RenderLayer::CEFormat::R16G16_Snorm: return DXGI_FORMAT_R16G16_SNORM;
		case Main::RenderLayer::CEFormat::R16G16_Sint: return DXGI_FORMAT_R16G16_SINT;
		case Main::RenderLayer::CEFormat::R32_Typeless: return DXGI_FORMAT_R32_TYPELESS;
		case Main::RenderLayer::CEFormat::D32_Float: return DXGI_FORMAT_D32_FLOAT;
		case Main::RenderLayer::CEFormat::R32_Float: return DXGI_FORMAT_R32_FLOAT;
		case Main::RenderLayer::CEFormat::R32_Uint: return DXGI_FORMAT_R32_UINT;
		case Main::RenderLayer::CEFormat::R32_Sint: return DXGI_FORMAT_R32_SINT;
		case Main::RenderLayer::CEFormat::R24G8_Typeless: return DXGI_FORMAT_R24G8_TYPELESS;
		case Main::RenderLayer::CEFormat::D24_Unorm_S8_Uint: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case Main::RenderLayer::CEFormat::R24_Unorm_X8_Typeless: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case Main::RenderLayer::CEFormat::X24_Typeless_G8_Uint: return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		case Main::RenderLayer::CEFormat::R8G8_Typeless: return DXGI_FORMAT_R8G8_TYPELESS;
		case Main::RenderLayer::CEFormat::R8G8_Unorm: return DXGI_FORMAT_R8G8_UNORM;
		case Main::RenderLayer::CEFormat::R8G8_Uint: return DXGI_FORMAT_R8G8_UINT;
		case Main::RenderLayer::CEFormat::R8G8_Snorm: return DXGI_FORMAT_R8G8_SNORM;
		case Main::RenderLayer::CEFormat::R8G8_Sint: return DXGI_FORMAT_R8G8_SINT;
		case Main::RenderLayer::CEFormat::R16_Typeless: return DXGI_FORMAT_R16_TYPELESS;
		case Main::RenderLayer::CEFormat::R16_Float: return DXGI_FORMAT_R16_FLOAT;
		case Main::RenderLayer::CEFormat::D16_Unorm: return DXGI_FORMAT_D16_UNORM;
		case Main::RenderLayer::CEFormat::R16_Unorm: return DXGI_FORMAT_R16_UNORM;
		case Main::RenderLayer::CEFormat::R16_Uint: return DXGI_FORMAT_R16_UINT;
		case Main::RenderLayer::CEFormat::R16_Snorm: return DXGI_FORMAT_R16_SNORM;
		case Main::RenderLayer::CEFormat::R16_Sint: return DXGI_FORMAT_R16_SINT;
		case Main::RenderLayer::CEFormat::R8_Typeless: return DXGI_FORMAT_R8_TYPELESS;
		case Main::RenderLayer::CEFormat::R8_Unorm: return DXGI_FORMAT_R8_UNORM;
		case Main::RenderLayer::CEFormat::R8_Uint: return DXGI_FORMAT_R8_UINT;
		case Main::RenderLayer::CEFormat::R8_Snorm: return DXGI_FORMAT_R8_SNORM;
		case Main::RenderLayer::CEFormat::R8_Sint: return DXGI_FORMAT_R8_SINT;
		default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	inline D3D12_INPUT_CLASSIFICATION ConvertInputClassification(CEInputClassification inputClassification) {
		switch (inputClassification) {
		case CEInputClassification::Instance:
			return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		case CEInputClassification::Vertex:
			return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		}

		return D3D12_INPUT_CLASSIFICATION();
	}

	inline D3D12_DEPTH_WRITE_MASK ConvertDepthWriteMask(CEDepthWriteMask depthWriteMask) {
		switch (depthWriteMask) {
		case CEDepthWriteMask::Zero:
			return D3D12_DEPTH_WRITE_MASK_ZERO;
		case CEDepthWriteMask::All:
			return D3D12_DEPTH_WRITE_MASK_ALL;
		}

		return D3D12_DEPTH_WRITE_MASK();
	}

	inline D3D12_COMPARISON_FUNC ConvertComparisonFunc(CEComparisonFunc comparisonFunc) {
		switch (comparisonFunc) {
		case CEComparisonFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
		case CEComparisonFunc::Less: return D3D12_COMPARISON_FUNC_LESS;
		case CEComparisonFunc::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
		case CEComparisonFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case CEComparisonFunc::Greater: return D3D12_COMPARISON_FUNC_GREATER;
		case CEComparisonFunc::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case CEComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case CEComparisonFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
		}

		return D3D12_COMPARISON_FUNC();
	}

	inline D3D12_STENCIL_OP ConvertStencilOp(CEStencilOp StencilOp) {
		switch (StencilOp) {
		case CEStencilOp::Keep: return D3D12_STENCIL_OP_KEEP;
		case CEStencilOp::Zero: return D3D12_STENCIL_OP_ZERO;
		case CEStencilOp::Replace: return D3D12_STENCIL_OP_REPLACE;
		case CEStencilOp::IncrSat: return D3D12_STENCIL_OP_INCR_SAT;
		case CEStencilOp::DecrSat: return D3D12_STENCIL_OP_DECR_SAT;
		case CEStencilOp::Invert: return D3D12_STENCIL_OP_INVERT;
		case CEStencilOp::Incr: return D3D12_STENCIL_OP_INCR;
		case CEStencilOp::Decr: return D3D12_STENCIL_OP_DECR;
		}

		return D3D12_STENCIL_OP();
	}

	// Converts DepthStencilOp to D3D12_DEPTH_STENCILOP_DESC
	inline D3D12_DEPTH_STENCILOP_DESC ConvertDepthStencilOp(const CEDepthStencilOp& DepthStencilOp) {
		return
		{
			ConvertStencilOp(DepthStencilOp.StencilFailOp),
			ConvertStencilOp(DepthStencilOp.StencilDepthFailOp),
			ConvertStencilOp(DepthStencilOp.StencilPassOp),
			ConvertComparisonFunc(DepthStencilOp.StencilFunc)
		};
	}

	// Converts ECullMode to D3D12_CULL_MODE
	inline D3D12_CULL_MODE ConvertCullMode(CECullMode CullMode) {
		switch (CullMode) {
		case CECullMode::Back: return D3D12_CULL_MODE_BACK;
		case CECullMode::Front: return D3D12_CULL_MODE_FRONT;
		default: return D3D12_CULL_MODE_NONE;
		}
	}

	// Converts EFillMode to D3D12_FILL_MODE
	inline D3D12_FILL_MODE ConvertFillMode(CEFillMode FillMode) {
		switch (FillMode) {
		case CEFillMode::Solid: return D3D12_FILL_MODE_SOLID;
		case CEFillMode::WireFrame: return D3D12_FILL_MODE_WIREFRAME;
		}

		return D3D12_FILL_MODE();
	}

	// Converts EBlendOp to D3D12_FILL_MODE
	inline D3D12_BLEND_OP ConvertBlendOp(CEBlendOp BlendOp) {
		switch (BlendOp) {
		case CEBlendOp::Add: return D3D12_BLEND_OP_ADD;
		case CEBlendOp::Max: return D3D12_BLEND_OP_MAX;
		case CEBlendOp::Min: return D3D12_BLEND_OP_MIN;
		case CEBlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case CEBlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
		}

		return D3D12_BLEND_OP();
	}

	// Converts EBlend to D3D12_BLEND
	inline D3D12_BLEND ConvertBlend(CEBlend Blend) {
		switch (Blend) {
		case CEBlend::Zero: return D3D12_BLEND_ZERO;
		case CEBlend::One: return D3D12_BLEND_ONE;
		case CEBlend::SrcColor: return D3D12_BLEND_SRC_COLOR;
		case CEBlend::InvSrcColor: return D3D12_BLEND_INV_SRC_COLOR;
		case CEBlend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
		case CEBlend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
		case CEBlend::DestAlpha: return D3D12_BLEND_DEST_ALPHA;
		case CEBlend::InvDestAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
		case CEBlend::DestColor: return D3D12_BLEND_DEST_COLOR;
		case CEBlend::InvDestColor: return D3D12_BLEND_INV_DEST_COLOR;
		case CEBlend::SrcAlphaSat: return D3D12_BLEND_SRC_ALPHA_SAT;
		case CEBlend::Src1Color: return D3D12_BLEND_SRC1_COLOR;
		case CEBlend::InvSrc1Color: return D3D12_BLEND_INV_SRC1_COLOR;
		case CEBlend::Src1Alpha: return D3D12_BLEND_SRC1_ALPHA;
		case CEBlend::InvSrc1Alpha: return D3D12_BLEND_INV_SRC1_ALPHA;
		case CEBlend::BlendFactor: return D3D12_BLEND_BLEND_FACTOR;
		case CEBlend::InvBlendFactor: return D3D12_BLEND_INV_BLEND_FACTOR;
		}

		return D3D12_BLEND();
	}

	// Converts ELogicOp to D3D12_LOGIC_OP
	inline D3D12_LOGIC_OP ConvertLogicOp(CELogicOp LogicOp) {
		switch (LogicOp) {
		case CELogicOp::Clear: return D3D12_LOGIC_OP_CLEAR;
		case CELogicOp::Set: return D3D12_LOGIC_OP_SET;
		case CELogicOp::Copy: return D3D12_LOGIC_OP_COPY;
		case CELogicOp::CopyInverted: return D3D12_LOGIC_OP_COPY_INVERTED;
		case CELogicOp::Noop: return D3D12_LOGIC_OP_NOOP;
		case CELogicOp::Invert: return D3D12_LOGIC_OP_INVERT;
		case CELogicOp::And: return D3D12_LOGIC_OP_AND;
		case CELogicOp::Nand: return D3D12_LOGIC_OP_NAND;
		case CELogicOp::Or: return D3D12_LOGIC_OP_OR;
		case CELogicOp::Nor: return D3D12_LOGIC_OP_NOR;
		case CELogicOp::Xor: return D3D12_LOGIC_OP_XOR;
		case CELogicOp::Equiv: return D3D12_LOGIC_OP_EQUIV;
		case CELogicOp::AndReverse: return D3D12_LOGIC_OP_AND_REVERSE;
		case CELogicOp::AndInverted: return D3D12_LOGIC_OP_AND_INVERTED;
		case CELogicOp::OrReverse: return D3D12_LOGIC_OP_OR_REVERSE;
		case CELogicOp::OrInverted: return D3D12_LOGIC_OP_OR_INVERTED;
		}

		return D3D12_LOGIC_OP();
	}

	// Converts RenderTargetWriteState to D3D12 RenderTargetWriteMask
	inline uint8 ConvertRenderTargetWriteState(const CERenderTargetWriteState& RenderTargetWriteState) {
		uint8 RenderTargetWriteMask = 0;
		if (RenderTargetWriteState.WriteAll()) {
			RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}
		else {
			if (RenderTargetWriteState.WriteRed()) {
				RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_RED;
			}
			if (RenderTargetWriteState.WriteGreen()) {
				RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
			}
			if (RenderTargetWriteState.WriteBlue()) {
				RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
			}
			if (RenderTargetWriteState.WriteAlpha()) {
				RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
			}
		}

		return RenderTargetWriteMask;
	}

	// Converts EPrimitiveTopologyType to D3D12_PRIMITIVE_TOPOLOGY_TYPE
	inline D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopologyType(CEPrimitiveTopologyType PrimitiveTopologyType) {
		switch (PrimitiveTopologyType) {
		case CEPrimitiveTopologyType::Line: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case CEPrimitiveTopologyType::Patch: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		case CEPrimitiveTopologyType::Point: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case CEPrimitiveTopologyType::Triangle: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case CEPrimitiveTopologyType::Undefined: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		}

		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}

	// Converts EPrimitiveTopology to D3D12_PRIMITIVE_TOPOLOGY
	inline D3D12_PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(CEPrimitiveTopology PrimitiveTopology) {
		switch (PrimitiveTopology) {
		case CEPrimitiveTopology::LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case CEPrimitiveTopology::LineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case CEPrimitiveTopology::PointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case CEPrimitiveTopology::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case CEPrimitiveTopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case CEPrimitiveTopology::Undefined: return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}

		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}

	// Converts EResourceState to D3D12_RESOURCE_STATES
	inline D3D12_RESOURCE_STATES ConvertResourceState(CEResourceState ResourceState) {
		switch (ResourceState) {
		case CEResourceState::Common: return D3D12_RESOURCE_STATE_COMMON;
		case CEResourceState::CopyDest: return D3D12_RESOURCE_STATE_COPY_DEST;
		case CEResourceState::CopySource: return D3D12_RESOURCE_STATE_COPY_SOURCE;
		case CEResourceState::DepthRead: return D3D12_RESOURCE_STATE_DEPTH_READ;
		case CEResourceState::DepthWrite: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
		case CEResourceState::IndexBuffer: return D3D12_RESOURCE_STATE_INDEX_BUFFER;
		case CEResourceState::NonPixelShaderResource: return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		case CEResourceState::PixelShaderResource: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		case CEResourceState::Present: return D3D12_RESOURCE_STATE_PRESENT;
		case CEResourceState::RenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		case CEResourceState::ResolveDest: return D3D12_RESOURCE_STATE_RESOLVE_DEST;
		case CEResourceState::ResolveSource: return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		case CEResourceState::ShadingRateSource: return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
		case CEResourceState::UnorderedAccess: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		case CEResourceState::VertexAndConstantBuffer: return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		case CEResourceState::GenericRead: return D3D12_RESOURCE_STATE_GENERIC_READ;
		}

		return D3D12_RESOURCE_STATES();
	}

	// Converts ESamplerMode to D3D12_TEXTURE_ADDRESS_MODE
	inline D3D12_TEXTURE_ADDRESS_MODE ConvertSamplerMode(CESamplerMode SamplerMode) {
		switch (SamplerMode) {
		case CESamplerMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case CESamplerMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case CESamplerMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case CESamplerMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		case CESamplerMode::MirrorOnce: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		}

		return D3D12_TEXTURE_ADDRESS_MODE();
	}

	// Converts ESamplerFilter to D3D12_FILTER
	inline D3D12_FILTER ConvertSamplerFilter(CESamplerFilter SamplerFilter) {
		switch (SamplerFilter) {
		case CESamplerFilter::MinMagMipPoint: return D3D12_FILTER_MIN_MAG_MIP_POINT;
		case CESamplerFilter::MinMagPoint_MipLinear: return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case CESamplerFilter::MinPoint_MagLinear_MipPoint: return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case CESamplerFilter::MinPoint_MagMipLinear: return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		case CESamplerFilter::MinLinear_MagMipPoint: return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case CESamplerFilter::MinLinear_MagPoint_MipLinear: return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case CESamplerFilter::MinMagLinear_MipPoint: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case CESamplerFilter::MinMagMipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		case CESamplerFilter::Anistrotopic: return D3D12_FILTER_ANISOTROPIC;
		case CESamplerFilter::Comparison_MinMagMipPoint: return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		case CESamplerFilter::Comparison_MinMagPoint_MipLinear: return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		case CESamplerFilter::Comparison_MinPoint_MagLinear_MipPoint: return
				D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case CESamplerFilter::Comparison_MinPoint_MagMipLinear: return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		case CESamplerFilter::Comparison_MinLinear_MagMipPoint: return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		case CESamplerFilter::Comparison_MinLinear_MagPoint_MipLinear: return
				D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case CESamplerFilter::Comparison_MinMagLinear_MipPoint: return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		case CESamplerFilter::Comparison_MinMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case CESamplerFilter::Comparison_Anistrotopic: return D3D12_FILTER_COMPARISON_ANISOTROPIC;
		}

		return D3D12_FILTER();
	}

	// Converts EShadingRate to D3D12_SHADING_RATE
	inline D3D12_SHADING_RATE ConvertShadingRate(CEShadingRate ShadingRate) {
		switch (ShadingRate) {
		case CEShadingRate::VRS_1x1: return D3D12_SHADING_RATE_1X1;
		case CEShadingRate::VRS_1x2: return D3D12_SHADING_RATE_1X2;
		case CEShadingRate::VRS_2x1: return D3D12_SHADING_RATE_2X1;
		case CEShadingRate::VRS_2x2: return D3D12_SHADING_RATE_2X2;
		case CEShadingRate::VRS_2x4: return D3D12_SHADING_RATE_2X4;
		case CEShadingRate::VRS_4x2: return D3D12_SHADING_RATE_4X2;
		case CEShadingRate::VRS_4x4: return D3D12_SHADING_RATE_4X4;
		}

		return D3D12_SHADING_RATE();
	}

	// Operators for D3D12_GPU_DESCRIPTOR_HANDLE
	inline bool operator==(D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHandle, uint64 Value) {
		return DescriptorHandle.ptr == Value;
	}

	inline bool operator!=(D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHandle, uint64 Value) {
		return !(DescriptorHandle == Value);
	}

	inline bool operator==(D3D12_GPU_DESCRIPTOR_HANDLE Left, D3D12_GPU_DESCRIPTOR_HANDLE Right) {
		return Left.ptr == Right.ptr;
	}

	inline bool operator!=(D3D12_GPU_DESCRIPTOR_HANDLE Left, D3D12_GPU_DESCRIPTOR_HANDLE Right) {
		return !(Left == Right);
	}

	// Operators for D3D12_CPU_DESCRIPTOR_HANDLE
	inline bool operator==(D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle, uint64 Value) {
		return DescriptorHandle.ptr == Value;
	}

	inline bool operator!=(D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle, uint64 Value) {
		return !(DescriptorHandle == Value);
	}

	inline bool operator==(D3D12_CPU_DESCRIPTOR_HANDLE Left, D3D12_CPU_DESCRIPTOR_HANDLE Right) {
		return Left.ptr == Right.ptr;
	}

	inline bool operator!=(D3D12_CPU_DESCRIPTOR_HANDLE Left, D3D12_CPU_DESCRIPTOR_HANDLE Right) {
		return !(Left == Right);
	}

	inline uint32 GetFormatStride(DXGI_FORMAT Format) {
		switch (Format) {
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT: {
			return 16;
		}

		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT: {
			return 12;
		}

		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: {
			return 8;
		}

		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT: {
			return 4;
		}

		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT: {
			return 2;
		}

		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM: {
			return 1;
		}

		default: {
			return 0;
		}
		}
	}

	inline DXGI_FORMAT CastShaderResourceFormat(DXGI_FORMAT Format) {
		switch (Format) {
		case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case DXGI_FORMAT_R32G32B32_TYPELESS: return DXGI_FORMAT_R32G32B32_FLOAT;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case DXGI_FORMAT_R32G32_TYPELESS: return DXGI_FORMAT_R32G32_FLOAT;
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS: return DXGI_FORMAT_R10G10B10A2_UNORM;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_R16G16_TYPELESS: return DXGI_FORMAT_R16G16_FLOAT;
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
		case DXGI_FORMAT_R24G8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case DXGI_FORMAT_R8G8_TYPELESS: return DXGI_FORMAT_R8G8_UNORM;
		case DXGI_FORMAT_R16_TYPELESS: return DXGI_FORMAT_R16_FLOAT;
		case DXGI_FORMAT_D16_UNORM: return DXGI_FORMAT_R16_UNORM;
		case DXGI_FORMAT_R8_TYPELESS: return DXGI_FORMAT_R8_UNORM;
		default: return Format;
		}
	}

	inline D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS ConvertAccelerationStructureBuildFlags(uint32 InFlags) {
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS Flags =
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
		if (InFlags & RayTracingStructureBuildFlag_AllowUpdate) {
			Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
		}
		if (InFlags & RayTracingStructureBuildFlag_PreferFastTrace) {
			Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		}
		if (InFlags & RayTracingStructureBuildFlag_PreferFastBuild) {
			Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
		}

		return Flags;
	}

	inline D3D12_RAYTRACING_INSTANCE_FLAGS ConvertRayTracingInstanceFlags(uint32 InFlags) {
		D3D12_RAYTRACING_INSTANCE_FLAGS Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		if (InFlags & RayTracingInstanceFlags_CullDisable) {
			Flags |= D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_CULL_DISABLE;
		}
		if (InFlags & RayTracingInstanceFlags_FrontCounterClockwise) {
			Flags |= D3D12_RAYTRACING_INSTANCE_FLAG_TRIANGLE_FRONT_COUNTERCLOCKWISE;
		}
		if (InFlags & RayTracingInstanceFlags_ForceOpaque) {
			Flags |= D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE;
		}
		if (InFlags & RayTracingInstanceFlags_ForceNonOpaque) {
			Flags |= D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_NON_OPAQUE;
		}

		return Flags;
	}
}
