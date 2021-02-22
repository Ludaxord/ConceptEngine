#pragma once
#include "../Game/CETimer.h"

namespace ConceptEngineFramework::Graphics {
	struct CEGraphicsManager;

	class CEPlayground {
	public:
		virtual ~CEPlayground() = default;

		virtual void Init(CEGraphicsManager* manager) = 0;
		virtual void Create() = 0;
		virtual void Update(const CETimer& gt) = 0;
		virtual void Render(const CETimer& gt) = 0;
	};
}
