#include "CEMonoCompiler.h"

using namespace ConceptEngine::Core::Compilers::Mono;

CEMonoCompiler::CEMonoCompiler(CEArray<CEMonoData>& MonoData): Domain(nullptr) {
	MonoArray = MonoData;
}

CEMonoCompiler::CEMonoCompiler(CEMonoData& MonoData): Domain(nullptr) {
	MonoArray.EmplaceBack(MonoData);
}

CEMonoCompiler::CEMonoCompiler(std::string& DllPath,
                               std::string& FilePath): Domain(nullptr) {
	MonoArray.EmplaceBack(CEMonoData(DllPath, FilePath));
}

CEMonoCompiler::CEMonoCompiler(CEArray<std::string> DllPaths, CEArray<std::string> CSFilePaths): Domain(nullptr) {
	Assert(DllPaths.Size() == CSFilePaths.Size());
	for (uint32 i = 0; i < CSFilePaths.Size(); i++) {
		MonoArray.EmplaceBack(CEMonoData(DllPaths[i], CSFilePaths[i]));
	}
}

bool CEMonoCompiler::Create() {
	//TODO: During installation process grab information from system environment (or install mono if is not available at user system). For now just pass Mono path.
	mono_set_dirs("C:\\Program Files\\Mono\\lib", "C:\\Program Files\\Mono\\etc");

	Domain = mono_jit_init("MonoScriptTry");
	if (!Domain) {
		CE_LOG_ERROR("[CEMonoCompiler] Failed to create Mono Domain")
		return false;
	}

	for (auto& MonoData : MonoArray) {
		MonoData.Assembly = CreateAssembly(MonoData.DllFilePath.c_str());
		if (!MonoData.Assembly) {
			CE_LOG_ERROR("[CEMonoCompiler] Failed to create Assembly for DllFile: " + MonoData.DllFilePath)
			return false;
		}

		MonoData.Image = CreateImage(MonoData.Assembly);
		if (!MonoData.Image) {
			CE_LOG_ERROR("[CEMonoCompiler] Failed to create Image for DllFile: " + MonoData.DllFilePath)
			return false;
		}
	}

	return true;
}

bool CEMonoCompiler::AddFilePath(std::string& InDllFilePath, std::string& InCSFilePath) {
	CEMonoData MonoData = CEMonoData(InDllFilePath, InCSFilePath);
	MonoData.Assembly = CreateAssembly(MonoData.DllFilePath.c_str());
	if (!MonoData.Assembly) {
		CE_LOG_ERROR("[CEMonoCompiler] Failed to create Assembly for DllFile: " + MonoData.DllFilePath)
		return false;
	}

	MonoData.Image = CreateImage(MonoData.Assembly);
	if (!MonoData.Image) {
		CE_LOG_ERROR("[CEMonoCompiler] Failed to create Image for DllFile: " + MonoData.DllFilePath)
		return false;
	}

	MonoArray.EmplaceBack(MonoData);

	return true;
}

MonoAssembly* CEMonoCompiler::CreateAssembly(const char* DllFilePath) const {
	return mono_domain_assembly_open(Domain, DllFilePath);
}

MonoImage* CEMonoCompiler::CreateImage(MonoAssembly* Assembly) const {
	return mono_assembly_get_image(Assembly);
}
