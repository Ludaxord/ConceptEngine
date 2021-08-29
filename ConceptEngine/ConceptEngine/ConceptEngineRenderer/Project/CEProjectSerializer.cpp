#include "CEProjectSerializer.h"

#include <fstream>

CEProjectSerializer::CEProjectSerializer(CEProject* Project): CurrentProject(Project) {
}

//TODO: Implement..
void CEProjectSerializer::Serialize(const std::string& FilePath) {
	Assert(false);
}

bool CEProjectSerializer::Deserialize(const std::string& FilePath) {
	std::ifstream Stream(FilePath);
	Assert(Stream);

	std::stringstream StringStream;
	StringStream << Stream.rdbuf();

	YAML::Node Data = YAML::Load(StringStream.str());
	if (!Data["Project"])
		return false;

	YAML::Node RootNode = Data["Project"];
	if (!RootNode["Name"])
		return false;

	CEProjectConfig& Config = CurrentProject->Config;
	Config.Name = RootNode["Name"].as<std::string>();

	Config.AssetDirectory = RootNode["AssetDirectory"].as<std::string>();
	Config.AssetRegistryPath = RootNode["AssetRegistry"].as<std::string>();

	Config.MeshPath = RootNode["MeshPath"].as<std::string>();
	Config.MeshSourcePath = RootNode["MeshSourcePath"].as<std::string>();

	Config.ScriptModulePath = RootNode["ScriptModulePath"].as<std::string>();

	Config.StartScene = RootNode["StartScene"].as<std::string>();

	Config.Scenes = CEArray(RootNode["Scenes"].as<std::vector<std::string>>());

	std::filesystem::path ProjectPath = FilePath;
	Config.ProjectDirectory = ProjectPath.parent_path().string();

	CurrentProject->OnDeserialized();

	return true;
}
