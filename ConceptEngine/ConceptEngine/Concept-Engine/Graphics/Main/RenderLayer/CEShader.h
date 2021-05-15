#pragma once

#include "CEResource.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	enum class CEShaderStage {
		Unknown = 0,
		Vertex = 1,
		Hull = 2,
		Domain = 3,
		Geometry = 4,
		Mesh = 5,
		Amplification = 6,
		Pixel = 7,
		Compute = 8,
		RayGen = 9,
		RayAnyHit = 10,
		RayClosestHit = 11,
		RayMiss = 12,
	};

	class CEShader : public CEResource {

	};

	class CEComputeShader : public CEShader {

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

	};

	class CERayClosestHitShader : public CEShader {

	};

	class CERayMissShader : public CEShader {

	};

}
