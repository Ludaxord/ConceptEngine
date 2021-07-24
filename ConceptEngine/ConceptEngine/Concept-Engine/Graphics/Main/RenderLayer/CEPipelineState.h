#pragma once
#include <string>

#include "CERendering.h"

#include "../../../Core/Containers/CEArray.h"
#include "../../../Core/Common/CETypes.h"
#include "CEResource.h"
#include "CEShader.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEPipelineState : public CEResource {
	public:
		virtual class CEGraphicsPipelineState* AsGraphics() {
			return nullptr;
		}

		virtual class CEComputePipelineState* AsCompute() {
			return nullptr;
		}

		virtual class CERayTracingPipelineState* AsRayTracing() {
			return nullptr;
		}

		virtual void DebugMessage() override {
			// CE_LOG_VERBOSE("[CEPipelineState]: Releasing Compute Pipeline State: " + GetName());
		}
	};

	enum class CEDepthWriteMask {
		Zero = 0,
		All = 1
	};

	enum class CEStencilOp {
		Keep = 1,
		Zero = 2,
		Replace = 3,
		IncrSat = 4,
		DecrSat = 5,
		Invert = 6,
		Incr = 7,
		Decr = 8
	};

	struct CEDepthStencilOp {
		CEStencilOp StencilFailOp = CEStencilOp::Keep;
		CEStencilOp StencilDepthFailOp = CEStencilOp::Keep;
		CEStencilOp StencilPassOp = CEStencilOp::Keep;
		CEComparisonFunc StencilFunc = CEComparisonFunc::Always;
	};

	enum class CECullMode {
		None = 1,
		Front = 2,
		Back = 3
	};

	enum class CEFillMode {
		WireFrame = 1,
		Solid = 2
	};

	enum class CEBlendOp {
		Add = 1,
		Subtract = 2,
		RevSubtract = 3,
		Min = 4,
		Max = 5
	};

	enum class CELogicOp {
		Clear = 0,
		Set = 1,
		Copy = 2,
		CopyInverted = 3,
		Noop = 4,
		Invert = 5,
		And = 6,
		Nand = 7,
		Or = 8,
		Nor = 9,
		Xor = 10,
		Equiv = 11,
		AndReverse = 12,
		AndInverted = 13,
		OrReverse = 14,
		OrInverted = 15
	};

	enum class CEBlend {
		Zero = 1,
		One = 2,
		SrcColor = 3,
		InvSrcColor = 4,
		SrcAlpha = 5,
		InvSrcAlpha = 6,
		DestAlpha = 7,
		InvDestAlpha = 8,
		DestColor = 9,
		InvDestColor = 10,
		SrcAlphaSat = 11,
		BlendFactor = 12,
		InvBlendFactor = 13,
		Src1Color = 14,
		InvSrc1Color = 15,
		Src1Alpha = 16,
		InvSrc1Alpha = 17
	};

	enum CEColorWriteFlag : uint8 {
		ColorWriteFlag_None = 0,
		ColorWriteFlag_Red = 1,
		ColorWriteFlag_Green = 2,
		ColorWriteFlag_Blue = 4,
		ColorWriteFlag_Alpha = 8,
		ColorWriteFlag_All = (((ColorWriteFlag_Red | ColorWriteFlag_Green) | ColorWriteFlag_Blue) |
			ColorWriteFlag_Alpha)
	};


	class CEDepthStencilState : public CEResource {
	};

	class CERasterizerState : public CEResource {
	};

	class CEBlendState : public CEResource {
	};

	class CEInputLayoutState : public CEResource {
	};

	class CEComputePipelineState : public CEPipelineState {
	public:
		virtual CEComputePipelineState* AsCompute() override {
			return this;
		}
	};

	class CEGraphicsPipelineState : public CEPipelineState {
	public:
		virtual CEGraphicsPipelineState* AsGraphics() override {
			return this;
		}
	};

	class CERayTracingPipelineState : public CEPipelineState {
	public:
		virtual CERayTracingPipelineState* AsRayTracing() override {
			return this;
		}
	};

	struct CEDepthStencilStateCreateInfo {
		CEDepthWriteMask DepthWriteMask = CEDepthWriteMask::All;
		CEComparisonFunc DepthFunc = CEComparisonFunc::Less;
		bool DepthEnable = true;
		uint8 StencilReadMask = 0xff;
		uint8 StencilWriteMask = 0xff;
		bool StencilEnable = false;
		CEDepthStencilOp FrontFace = CEDepthStencilOp();
		CEDepthStencilOp BackFace = CEDepthStencilOp();
	};

	struct CERasterizerStateCreateInfo {
		CEFillMode FillMode = CEFillMode::Solid;
		CECullMode CullMode = CECullMode::Back;
		bool FrontCounterClockwise = false;
		int32 DepthBias = 0;
		float DepthBiasClamp = 0.0f;
		float SlopeScaledDepthBias = 0.0f;
		bool DepthClipEnable = true;
		bool MultisampleEnable = false;
		bool AntialiasedLineEnable = false;
		uint32 ForcedSampleCount = 0;
		bool EnableConservativeRaster = false;
	};

	struct CERenderTargetWriteState {
		CERenderTargetWriteState() = default;

		CERenderTargetWriteState(uint8 mask) : Mask(mask) {

		}

		bool WriteNone() const {
			return Mask == ColorWriteFlag_None;
		}

		bool WriteRed() const {
			return (Mask & ColorWriteFlag_Red);
		}

		bool WriteGreen() const {
			return (Mask & ColorWriteFlag_Green);
		}

		bool WriteBlue() const {
			return (Mask & ColorWriteFlag_Blue);
		}

		bool WriteAlpha() const {
			return (Mask & ColorWriteFlag_Alpha);
		}

		bool WriteAll() const {
			return Mask == ColorWriteFlag_All;
		}

		uint8 Mask = ColorWriteFlag_All;
	};

	struct CERenderTargetBlendState {
		CEBlend SourceBlend = CEBlend::One;
		CEBlend DestinationBlend = CEBlend::Zero;
		CEBlendOp BlendOp = CEBlendOp::Add;
		CEBlend SourceBlendAlpha = CEBlend::One;
		CEBlend DestinationBlendAlpha = CEBlend::Zero;
		CEBlendOp BlendOpAlpha = CEBlendOp::Add;
		CELogicOp LogicOp = CELogicOp::Noop;
		bool BlendEnable = false;
		bool LogicOpEnable = false;
		CERenderTargetWriteState RenderTargetWriteMask;
	};

	struct CEBlendStateCreateInfo {
		bool AlphaToCoverageEnable = false;
		bool IndependentBlendEnable = false;
		CERenderTargetBlendState RenderTarget[8];
	};

	enum class CEInputClassification { Vertex = 0, Instance = 1 };

	struct CEInputElement {
		std::string Semantic = "";
		uint32 SemanticIndex = 0;
		CEFormat Format = CEFormat::Unknown;
		uint32 InputSlot = 0;
		uint32 ByteOffset = 0;
		CEInputClassification InputClassification = CEInputClassification::Vertex;
		uint32 InstanceStepRate = 0;
	};

	struct CEInputLayoutStateCreateInfo {
		CEInputLayoutStateCreateInfo() = default;

		CEInputLayoutStateCreateInfo(const CEArray<CEInputElement>& elements): Elements(elements) {
		}

		CEInputLayoutStateCreateInfo(std::initializer_list<CEInputElement> list) : Elements(list) {

		}

		CEArray<CEInputElement> Elements;
	};

	struct CEPipelineRenderTargetFormats {
		CEFormat RenderTargetFormats[8];
		uint32 NumRenderTargets = 0;
		CEFormat DepthStencilFormat = CEFormat::Unknown;
	};

	struct CEGraphicsPipelineShaderState {
		CEGraphicsPipelineShaderState() = default;

		CEGraphicsPipelineShaderState(CEVertexShader* vertexShader, CEPixelShader* pixelShader):
			VertexShader(vertexShader), PixelShader(pixelShader) {

		}

		CEVertexShader* VertexShader = nullptr;
		CEPixelShader* PixelShader = nullptr;
	};

	enum class CEIndexBufferStripCutValue {
		Disabled = 0,
		_0xffff = 1,
		_0xffffffff = 2
	};

	struct CEGraphicsPipelineStateCreateInfo {
		CEInputLayoutState* InputLayoutState = nullptr;
		CEDepthStencilState* DepthStencilState = nullptr;
		CERasterizerState* RasterizerState = nullptr;
		CEBlendState* BlendState = nullptr;
		uint32 SampleCount = 1;
		uint32 SampleQuality = 0;
		uint32 SampleMask = 0xffffffff;
		CEIndexBufferStripCutValue IBStripCutValue = CEIndexBufferStripCutValue::Disabled;
		CEPrimitiveTopologyType PrimitiveTopologyType = CEPrimitiveTopologyType::Triangle;
		CEGraphicsPipelineShaderState ShaderState;
		CEPipelineRenderTargetFormats PipelineFormats;
	};

	struct CEComputePipelineStateCreateInfo {
		CEComputePipelineStateCreateInfo() = default;

		CEComputePipelineStateCreateInfo(CEComputeShader* shader): Shader(shader) {

		}

		CEComputeShader* Shader = nullptr;
	};

	struct CERayTracingHitGroup {
		CERayTracingHitGroup() = default;

		CERayTracingHitGroup(const std::string& name, CERayAnyHitShader* anyHitShader,
		                     CERayClosestHitShader* closestHitShader): Name(name),
		                                                               AnyHit(anyHitShader),
		                                                               ClosestHit(closestHitShader) {

		}

		std::string Name;
		CERayAnyHitShader* AnyHit;
		CERayClosestHitShader* ClosestHit;
	};

	struct CERayTracingPipelineStateCreateInfo {
		CERayGenShader* RayGen = nullptr;
		CEArray<CERayAnyHitShader*> AnyHitShaders;
		CEArray<CERayClosestHitShader*> ClosestHitShaders;
		CEArray<CERayMissShader*> MissShaders;
		CEArray<CERayTracingHitGroup> HitGroups;
		uint32 MaxAttributeSizeInBytes = 0;
		uint32 MaxPayloadSizeInBytes = 0;
		uint32 MaxRecursionDepth = 1;
	};
}
