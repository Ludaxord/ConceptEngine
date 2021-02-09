#pragma once

#include <memory>

#include "CESubObject.h"

namespace Concept::GraphicsEngine::Direct3D {
	class CEDevice;

	class CEPipelineConfig : public CESubObject {
	public:
	protected:
		friend class CEDevice;
		friend class std::default_delete<CEPipelineConfig>;

		CEPipelineConfig(uint32_t maxTraceRecursionDepth);

	private:
		D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig = {};
	};
}
