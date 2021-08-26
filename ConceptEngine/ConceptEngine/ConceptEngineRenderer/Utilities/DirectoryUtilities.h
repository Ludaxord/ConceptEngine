#pragma once
#include <direct.h>
#include <filesystem>
#include <fstream>
#include <stdio.h>  /* defines FILENAME_MAX */

#include "Platform/Generic/FileSystem/CEFileBuffer.h"

//TODO: Move all methods to CEFileSystem class...
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


inline std::filesystem::path GetCacheDirectory() {
	return std::filesystem::path(GetMainDirectory()) / "Cache";
}

inline void CreateCacheDirectory(std::string Path) {
	std::filesystem::path CacheDirectory = GetCacheDirectory() / Path;
	if (!std::filesystem::exists(CacheDirectory))
		std::filesystem::create_directories(CacheDirectory);
}

//NOTE: Temporary method for creating file...
inline bool WriteBytes(const std::filesystem::path& FilePath, const CEFileBuffer Buffer) {
	std::ofstream Stream(FilePath, std::ios::binary | std::ios::trunc);

	if (!Stream) {
		Stream.close();
		return false;
	}

	Stream.write((char*)Buffer.Data, Buffer.Size);
	Stream.close();

	return true;
}

inline CEFileBuffer ReadBytes(const std::filesystem::path& FilePath) {
	CEFileBuffer Buffer;

	std::ifstream Stream(FilePath, std::ios::binary | std::ios::ate);
	Assert(Stream);

	std::streampos End = Stream.tellg();
	Stream.seekg(0, std::ios::beg);
	uint32 Size = End - Stream.tellg();
	Assert(Size != 0);

	Buffer.Allocate(Size);
	Stream.read((char*)Buffer.Data, Buffer.Size);

	return Buffer;
}
