#pragma once

#include "../../Log/CELog.h"
#include "../CECompiler.h"
#include "../../Containers/CEArray.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace ConceptEngine::Core::Compilers::Mono {
	struct CEMonoData {
		CEMonoData() = default;

		CEMonoData(MonoAssembly* InAssembly, MonoImage* InImage): Assembly(InAssembly), Image(InImage) {

		}

		CEMonoData(MonoAssembly* InAssembly,
		           MonoImage* InImage,
		           std::string& InDllFilePath,
		           std::string& InCSFilePath): CEMonoData(InAssembly, InImage) {
			DllFilePath = InDllFilePath;
			CSFilePath = InCSFilePath;
		}

		CEMonoData(std::string& InDllFilePath, std::string& InCSFilePath) {
			DllFilePath = InDllFilePath;
			CSFilePath = InCSFilePath;
		}

		MonoAssembly* Assembly = nullptr;
		MonoImage* Image = nullptr;
		std::string DllFilePath;
		std::string CSFilePath;
	};

	class CEMonoCompiler : public CECompiler {
	public:
		CEMonoCompiler(CEArray<CEMonoData>& MonoData);
		CEMonoCompiler(CEMonoData& MonoData);
		CEMonoCompiler(std::string& DllPath, std::string& FilePath);
		CEMonoCompiler(CEArray<std::string> DllPaths, CEArray<std::string> FilePaths);

		bool Create();

		bool AddFilePath(std::string& InDllFilePath, std::string& InCSFilePath);

	private:
		MonoAssembly* CreateAssembly(const char* DllFilePath) const;
		MonoImage* CreateImage(MonoAssembly* Assembly) const;

	private:
		MonoDomain* Domain;
		CEArray<CEMonoData> MonoArray;
	};
}
