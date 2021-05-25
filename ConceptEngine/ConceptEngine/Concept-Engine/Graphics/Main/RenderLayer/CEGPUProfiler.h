#pragma once
#include "CEResource.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	struct TimeQuery {
		uint64 Begin;
		uint64 End;
	};

	class CEGPUProfiler : public CEResource {
	public:
		virtual void GetTimeQuery(TimeQuery& query, uint32 index) const = 0;
		virtual uint64 GetFrequency() const = 0;
	};
}
