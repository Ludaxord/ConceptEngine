#pragma once
#include "../CEPlayground.h"

namespace ConceptEngineFramework::Graphics::OpenGL {
	class CEOGLPlayground : public CEPlayground {
	public:
		virtual void Init(CEGraphicsManager* manager) override;
		virtual void Update(const CETimer& gt) override;
		virtual void Render(const CETimer& gt) override;
	};
}
