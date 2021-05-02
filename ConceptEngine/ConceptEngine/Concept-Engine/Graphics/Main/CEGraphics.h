#pragma once
#include <combaseapi.h>

namespace ConceptEngine::Graphics::Main {
	interface CEGraphics {
	public:
		virtual ~CEGraphics() = default;
		virtual void Create() = 0;
		virtual void CreateManagers() = 0;
	protected:
	private:
	};
}
