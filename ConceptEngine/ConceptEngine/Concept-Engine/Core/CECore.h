#pragma once
#include "../Compilers/CECompiler.h"

namespace ConceptEngine::Core {
	class CECore {
	public:
		CECore() = default;
		~CECore() = default;

		virtual int Run() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
 	
	protected:
		Compilers::CECompiler m_compiler;

	private:
	};
}
