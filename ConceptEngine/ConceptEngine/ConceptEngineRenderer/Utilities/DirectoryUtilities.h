#pragma once
#include <direct.h>
#include <filesystem>
#include <stdio.h>  /* defines FILENAME_MAX */

inline std::string GetProjectDirectory() {
	char cCurrentPath[FILENAME_MAX];
#define GET_CURRENT_DIR _getcwd
	if (!GET_CURRENT_DIR(cCurrentPath, sizeof(cCurrentPath))) {
		return "";
	}


	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	return cCurrentPath;
}

inline std::string GetMainDirectory() {
	namespace fs = std::filesystem;
	auto dir = GetProjectDirectory();
	std::filesystem::path working_dir(dir);
	return working_dir.parent_path().string();
}

inline std::string GetGraphicsContentDirectory(std::string path) {
	return GetMainDirectory() + "\\ConceptEngineRenderer\\" + path;
}

inline std::string GetEngineSourceDirectory(std::string path) {
	return GetMainDirectory() + "\\ConceptEngineRenderer\\" + path;
}
