#pragma once

#include <map>
#include <vector>

#include "CENsightAftermathHelpers.h"

namespace ConceptEngine::Core::Debug::Nsight {
	class CENsightAftermathShaderDatabase {
	public:
		CENsightAftermathShaderDatabase();
		~CENsightAftermathShaderDatabase();

		bool FindShaderBinary(const GFSDK_Aftermath_ShaderHash& ShaderHash, std::vector<uint8_t>& Shader) const;

		bool FindShaderBinary(const GFSDK_Aftermath_ShaderInstructionsHash& ShaderInstructionsHash,
		                      std::vector<uint8_t>& Shader) const;
		bool FindSourceShaderDebugData(const GFSDK_Aftermath_ShaderDebugName& ShaderDebugName,
		                               std::vector<uint8_t>& DebugData) const;

	private:
		void AddShaderBinary(const char* FilePath);

		void AddSourceShaderDebugData(const char* FilePath, const char* FileName);

		static bool ReadFile(const char* FileName, std::vector<uint8_t>& Data);

		std::map<GFSDK_Aftermath_ShaderHash, std::vector<uint8_t>> ShaderBinaries;

		std::map<GFSDK_Aftermath_ShaderInstructionsHash, GFSDK_Aftermath_ShaderHash> ShaderInstructionsToShaderHash;

		std::map<GFSDK_Aftermath_ShaderDebugName, std::vector<uint8_t>> SourceShaderDebugData;
	};
}
