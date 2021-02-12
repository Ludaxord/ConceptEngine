#pragma once
#include <activation.h>
#include <memory>


namespace Concept::Engine {
	class ConceptEngine {
	public:
		ConceptEngine(HINSTANCE hInstance);

		virtual ~ConceptEngine() = default;

		int Run();
	};
}
