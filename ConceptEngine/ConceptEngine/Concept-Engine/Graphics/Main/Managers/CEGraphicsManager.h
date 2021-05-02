#pragma once
#include <combaseapi.h>

namespace ConceptEngine::Graphics::Main::Managers {
	class CEGraphicsManager {
	public:
		virtual ~CEGraphicsManager() = default;
		CEGraphicsManager() {}
		virtual void Create() = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
		virtual void Resize() = 0;
	};
}
