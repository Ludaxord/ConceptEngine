#pragma once

#include "../CECompiler.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace ConceptEngine::Core::Compilers::Mono {
	class CEMonoCompiler : public CECompiler {
	public:
		CEMonoCompiler();

	void Create();
	protected:
	private:
		MonoDomain* Domain;
		MonoAssembly* Assembly;
		MonoImage* Image;
	};
}
