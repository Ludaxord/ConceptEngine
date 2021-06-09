#pragma once
#include <combaseapi.h>

namespace ConceptEngine::Core::Compilers {
	enum class Language {
		Cpp,
		CSharp,
		JS,
		Python,
		Schematics,
		None,
		Count
	};

	interface CECompiler {
	};
}
