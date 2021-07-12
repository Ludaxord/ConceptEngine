#include "CEMonoCompiler.h"

using namespace ConceptEngine::Core::Compilers::Mono;

CEMonoCompiler::CEMonoCompiler() {
}

void CEMonoCompiler::Create() {
	//TODO: During installation process grab information from system environment (or install mono if is not available at user system). For now just pass Mono path.
	mono_set_dirs("C:\\Program Files\\Mono\\lib", "C:\\Program Files\\Mono\\etc");

}
