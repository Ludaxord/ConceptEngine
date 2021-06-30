#pragma once
#include <direct.h>
#include <filesystem>
#include <stdio.h>  /* defines FILENAME_MAX */

#include "../Core/Platform/CEPlatformActions.h"

inline std::string GetProjectDirectory() {
	char cCurrentPath[FILENAME_MAX];
#ifdef WINDOWS_PLATFORM
#define GET_CURRENT_DIR _getcwd
	if (!GET_CURRENT_DIR(cCurrentPath, sizeof(cCurrentPath))) {
		return "";
	}
#else
#include <unistd.h>
#define GET_CURRENT_DIR getcwd
#endif

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
	return GetMainDirectory() + "\\Concept-Engine\\Graphics\\" + path;
}

inline std::string GetEngineSourceDirectory(std::string path) {
	return GetMainDirectory() + "\\Concept-Engine\\" + path;
}
