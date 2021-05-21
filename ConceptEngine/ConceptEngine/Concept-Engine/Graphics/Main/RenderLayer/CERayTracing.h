#pragma once
#include "CEResource.h"

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

	};

	class CERayTracingGeometry : public CEResource {

	};

	struct CERayTracingShaderResources {

	};

	struct CERayTracingGeometryInstance {

	};
}
