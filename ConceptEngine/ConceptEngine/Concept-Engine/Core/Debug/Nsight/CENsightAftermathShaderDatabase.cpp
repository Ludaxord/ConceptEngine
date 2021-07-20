#include <d3d12.h>
#include <fstream>

#include <GFSDK_Aftermath_GpuCrashDumpDecoding.h>
#include <GFSDK_Aftermath.h>
#include <GFSDK_Aftermath_GpuCrashDump.h>
#include <GFSDK_Aftermath_Defines.h>

#include "CENsightAftermathShaderDatabase.h"


using namespace ConceptEngine::Core::Debug::Nsight;

CENsightAftermathShaderDatabase::CENsightAftermathShaderDatabase(): ShaderBinaries(), ShaderInstructionsToShaderHash(),
                                                                    SourceShaderDebugData() {

	const char* shaderBinaryDirs[] = {
		// SHADERS_DIR,
		"Shaders"
	};
	for (size_t n = 0; n < sizeof(shaderBinaryDirs) / sizeof(const char*); ++n) {
		const std::string filePattern = std::string(shaderBinaryDirs[n]) + "/*.cso";

		WIN32_FIND_DATAA findData;
		HANDLE hFind = FindFirstFileA(filePattern.c_str(), &findData);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				const std::string filePath = std::string(shaderBinaryDirs[n]) + "/" + findData.cFileName;
				AddShaderBinary(filePath.c_str());
			}
			while (FindNextFileA(hFind, &findData) != 0);
			FindClose(hFind);
		}
	}

	// Add shader debug data to database
	const char* shaderDebugDataDirs[] = {
		// SHADERS_DIR,
		"Shaders"
	};
	const char* shaderDebugDataFileExts[] = {".lld", ".pdb"};
	for (size_t n = 0; n < sizeof(shaderDebugDataDirs) / sizeof(const char*); ++n) {
		for (size_t m = 0; m < sizeof(shaderDebugDataFileExts) / sizeof(const char*); ++m) {
			const std::string filePattern = std::string(shaderDebugDataDirs[n]) + "/*" + shaderDebugDataFileExts[m];

			WIN32_FIND_DATAA findData;
			HANDLE hFind = FindFirstFileA(filePattern.c_str(), &findData);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					const std::string filePath = std::string(shaderDebugDataDirs[n]) + "/" + findData.cFileName;
					AddSourceShaderDebugData(filePath.c_str(), findData.cFileName);
				}
				while (FindNextFileA(hFind, &findData) != 0);
				FindClose(hFind);
			}
		}
	}
}

CENsightAftermathShaderDatabase::~CENsightAftermathShaderDatabase() {
}

bool CENsightAftermathShaderDatabase::FindShaderBinary(const GFSDK_Aftermath_ShaderHash& ShaderHash,
                                                       std::vector<uint8_t>& Shader) const {
	auto shader = ShaderBinaries.find(ShaderHash);
	if (shader == ShaderBinaries.end()) {
		return false;
	}

	Shader = shader->second;
	return true;

}

bool CENsightAftermathShaderDatabase::FindShaderBinary(
	const GFSDK_Aftermath_ShaderInstructionsHash& ShaderInstructionsHash, std::vector<uint8_t>& Shader) const {
	auto shaderHash = ShaderInstructionsToShaderHash.find(ShaderInstructionsHash);
	if (shaderHash == ShaderInstructionsToShaderHash.end()) {
		return false;
	}

	const GFSDK_Aftermath_ShaderHash& ShaderHash = shaderHash->second;
	return FindShaderBinary(ShaderHash, Shader);
}

bool CENsightAftermathShaderDatabase::FindSourceShaderDebugData(const GFSDK_Aftermath_ShaderDebugName& ShaderDebugName,
                                                                std::vector<uint8_t>& DebugData) const {
	auto data = SourceShaderDebugData.find(ShaderDebugName);
	if (data == SourceShaderDebugData.end()) {
		return false;
	}

	DebugData = data->second;
	return true;
}

void CENsightAftermathShaderDatabase::AddShaderBinary(const char* FilePath) {
	std::vector<uint8_t> data;
	if (!ReadFile(FilePath, data)) {
		return;
	}

	const D3D12_SHADER_BYTECODE shader{data.data(), data.size()};
	GFSDK_Aftermath_ShaderHash shaderHash;
	GFSDK_Aftermath_ShaderInstructionsHash shaderInstructionsHash;
	AFTERMATH_CHECK_ERROR(
		GFSDK_Aftermath_GetShaderHash(
			GFSDK_Aftermath_Version_API,
			&shader,
			&shaderHash,
			&shaderInstructionsHash
		)
	);

	ShaderBinaries[shaderHash].swap(data);
	ShaderInstructionsToShaderHash[shaderInstructionsHash] = shaderHash;
}

void CENsightAftermathShaderDatabase::AddSourceShaderDebugData(const char* FilePath, const char* FileName) {
	std::vector<uint8_t> Data;
	if (!ReadFile(FilePath, Data)) {
		return;
	}

	GFSDK_Aftermath_ShaderDebugName DebugName;
	strncpy_s(DebugName.name, FileName, sizeof(DebugName.name) - 1);

	SourceShaderDebugData[DebugName].swap(Data);
}

bool CENsightAftermathShaderDatabase::ReadFile(const char* FileName, std::vector<uint8_t>& Data) {
	std::ifstream fs(FileName, std::ios::in | std::ios::binary);
	if (!fs) {
		return false;
	}

	fs.seekg(0, std::ios::end);
	Data.resize(fs.tellg());
	fs.seekg(0, std::ios::beg);
	fs.read(reinterpret_cast<char*>(Data.data()), Data.size());
	fs.close();

	return true;
}
