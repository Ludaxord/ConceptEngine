#pragma once

#include <string>
#include <unordered_map>

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
		CEMonoCompiler(std::unordered_map<std::string&, std::string&> FilePaths);

		bool Create();

		void AddFilePath(std::pair<std::string&, std::string&> FilePath);

	private:
		MonoAssembly* CreateAssembly(const char* DllFilePath) const;
		MonoImage* CreateImage(MonoAssembly* Assembly) const;

	private:
		MonoDomain* Domain;
		CEArray<CEMonoData> MonoArray;
	};
}
