#pragma once
#include "CEBuffer.h"
#include "CEResource.h"
#include "CEResourceViews.h"
#include "CESamplerState.h"
#include "../../../Core/Containers/CEArray.h"
#include "../../../Math/CEOperators.h"


namespace ConceptEngine::Graphics::Main::RenderLayer {
	enum CERayTracingStructureBuildFlag {
		RayTracingStructureBuildFlag_None = 0x0,
		RayTracingStructureBuildFlag_AllowUpdate = FLAG(1),
		RayTracingStructureBuildFlag_PreferFastTrace = FLAG(2),
		RayTracingStructureBuildFlag_PreferFastBuild = FLAG(3),
	};

	enum CERayTracingInstanceFlags {
		RayTracingInstanceFlags_None = 0,
		RayTracingInstanceFlags_CullDisable = FLAG(1),
		RayTracingInstanceFlags_FrontCounterClockwise = FLAG(2),
		RayTracingInstanceFlags_ForceOpaque = FLAG(3),
		RayTracingInstanceFlags_ForceNonOpaque = FLAG(4),
	};

	class CERayTracingScene : public CEResource {
	public:
		CERayTracingScene(uint32 flags) : Flags(flags) {

		}

		virtual CEShaderResourceView* GetShaderResourceView() const = 0;

		uint32 GetFlags() const {
			return Flags;
		}

	private:
		uint32 Flags;
	};

	class CERayTracingGeometry : public CEResource {
	public:
		CERayTracingGeometry(uint32 flags) : Flags(flags) {

		}

		uint32 GetFlags() const {
			return Flags;
		}

	private:
		uint32 Flags;
	};

	struct CERayTracingShaderResources {
		void AddConstantBuffer(CEConstantBuffer* buffer) {
			ConstantBuffers.EmplaceBack(buffer);
		}

		void AddShaderResourceView(CEShaderResourceView* view) {
			ShaderResourceViews.EmplaceBack(view);
		}

		void AddUnorderedAccessView(CEUnorderedAccessView* view) {
			UnorderedAccessViews.EmplaceBack(view);
		}

		void AddSamplerState(CESamplerState* samplerState) {
			SamplerStates.EmplaceBack(samplerState);
		}

		uint32 NumResources() const {
			return ConstantBuffers.Size() + ShaderResourceViews.Size() + UnorderedAccessViews.Size();
		}

		uint32 NumSamplers() const {
			return SamplerStates.Size();
		}

		void Reset() {
			ConstantBuffers.Clear();
			ShaderResourceViews.Clear();
			UnorderedAccessViews.Clear();
			SamplerStates.Clear();
		}

		std::string Identifier;
		CEArray<CEConstantBuffer*> ConstantBuffers;
		CEArray<CEShaderResourceView*> ShaderResourceViews;
		CEArray<CEUnorderedAccessView*> UnorderedAccessViews;
		CEArray<CESamplerState*> SamplerStates;
	};

	struct CERayPayload {
		//TODO: Comment before figure out way to fix bug with importing header
		CEVectorFloat3 Color;
		uint32 CurrentDepth;
	};

	struct CERayIntersectionAttributes {
		float Attribute0;
		float Attribute1;
	};

	struct CERayTracingGeometryInstance {
		CERef<CERayTracingGeometry> Instance;
		uint32 InstanceIndex = 0;
		uint32 HitGroupIndex = 0;
		uint32 Flags = RayTracingInstanceFlags_None;
		uint32 Mask = 0xff;
		CEMatrixFloat3X4 Transform;
	};
}
