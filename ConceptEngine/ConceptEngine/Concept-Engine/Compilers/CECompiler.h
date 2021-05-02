#pragma once
#include <combaseapi.h>

namespace ConceptEngine::Compilers {
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
		friend class CECore;
	};
}
