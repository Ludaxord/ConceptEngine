#pragma once
#include <filesystem>

class CEProject {
public:
	CEProject();
	~CEProject();

	const CEProjectConfig& GetConfig() const {
		return Config;
	}

	static CEProject* GetActive() {
		return ActiveProject;
	}

	static void SetActive(CEProject* Project);

	static const std::string& GetProjectName() {
		Assert(ActiveProject);
		return ActiveProject->GetConfig().Name;
	}

	static std::filesystem::path GetProjectDirectory() {
		Assert(ActiveProject);
		return ActiveProject->GetConfig().ProjectDirectory;
	}

	static std::filesystem::path GetAssetDirectory() {
		Assert(ActiveProject);
		return ActiveProject->GetConfig().AssetDirectory;
	}

	static std::filesystem::path GetAssetRegistryPath() {
		Assert(ActiveProject);
		return std::filesystem::path(ActiveProject->GetConfig().ProjectDirectory) / ActiveProject->GetConfig().
			AssetRegistryPath;
	}

	static std::filesystem::path GetMeshPath() {
		Assert(ActiveProject);
		return std::filesystem::path(ActiveProject->GetConfig().ProjectDirectory) / ActiveProject->GetConfig().MeshPath;
	}

	static std::filesystem::path GetCacheDirectory() {
		Assert(ActiveProject);
		return std::filesystem::path(ActiveProject->GetConfig().ProjectDirectory) / "Cache";
	}

	static std::filesystem::path GetScriptModulePath() {
		Assert(ActiveProject);
		return std::filesystem::path(ActiveProject->GetConfig().ProjectDirectory) / ActiveProject->GetConfig().
			ScriptModulePath;
	}

	static std::filesystem::path GetScriptModuleFilePath() {
		Assert(ActiveProject);
		std::string ProjectDll = GetProjectName() + std::string(".dll");
		return GetScriptModulePath() / ProjectDll;
	}

private:
	void OnDeserialized();

	CEProjectConfig Config;

	inline static CEProject* ActiveProject = nullptr;
};
