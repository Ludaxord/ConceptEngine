#pragma once

#include "CEResource.h"
#include "../../../Math/CEOperators.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	struct CEShaderParameterInfo {
		uint32 NumConstantBuffers = 0;
		uint32 NumShaderResourceViews = 0;
		uint32 NumUnorderedAccessViews = 0;
		uint32 NumSamplerStates = 0;
	};

	class CEShader : public CEResource {
	public:
		virtual class CEVertexShader* AsVertexShader() {
			return nullptr;
		}

		virtual class CEHullShader* AsHullShader() {
			return nullptr;
		}

		virtual class CEDomainShader* AsDomainShader() {
			return nullptr;
		}

		virtual class CEGeometryShader* AsGeometryShader() {
			return nullptr;
		}

		virtual class CEMeshShader* AsMeshShader() {
			return nullptr;
		}

		virtual class CEMeshShader* AsAmplificationShader() {
			return nullptr;
		}

		virtual class CEPixelShader* AsPixelShader() {
			return nullptr;
		}

		virtual class CEComputeShader* AsComputeShader() {
			return nullptr;
		}

		virtual class CERayGenShader* AsRayGenShader() {
			return nullptr;
		}

		virtual class CERayAnyHitShader* AsRayAnyHitShader() {
			return nullptr;
		}

		virtual class CERayClosestHitShader* AsRayClosestHitShader() {
			return nullptr;
		}

		virtual class CERayMissShader* AsRayMissShader() {
			return nullptr;
		}

		virtual void GetShaderParameterInfo(CEShaderParameterInfo& shaderParameterInfo) const = 0;

		virtual bool GetConstantBufferIndexByName(const std::string& name, uint32& index) const = 0;
		virtual bool GetUnorderedAccessViewIndexByName(const std::string& name, uint32& index) const = 0;
		virtual bool GetShaderResourceViewIndexByName(const std::string& name, uint32& index) const = 0;
		virtual bool GetSamplerIndexByName(const std::string& name, uint32& index) const = 0;
	};

	class CEComputeShader : public CEShader {
	public:
		virtual CEComputeShader* AsComputeShader() override {
			return this;
		};

		virtual CEVectorUint3 GetThreadGroupXYZ() const = 0;
	};

	class CEVertexShader : public CEShader {

	};

	class CEHullShader : public CEShader {

	};

	class CEDomainShader : public CEShader {

	};

	class CEGeometryShader : public CEShader {

	};

	class CEMeshShader : public CEShader {

	};

	class CEAmplificationShader : public CEShader {

	};

	class CEPixelShader : public CEShader {

	};

	class CERayGenShader : public CEShader {

	};

	class CERayAnyHitShader : public CEShader {
		virtual CERayAnyHitShader* AsRayAnyHitShader() override {
			return this;
		}
	};

	class CERayClosestHitShader : public CEShader {
		virtual CERayClosestHitShader* AsRayClosestHitShader() override {
			return this;
		}
	};

	class CERayMissShader : public CEShader {

	};
}
