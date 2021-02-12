#pragma once
#include <activation.h>
#include <memory>


// using namespace Concept::GameEngine;
// using namespace Concept::GraphicsEngine::Direct3D;

namespace Concept {
	class ConceptEngine {
	public:
		ConceptEngine(HINSTANCE hInstance);

		virtual ~ConceptEngine();

		int Run();
	protected:
		int RunEngine();
	private:
	};
}
