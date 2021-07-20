#include "CENsightAftermathShaderDatabase.h"

using namespace ConceptEngine::Core::Debug::Nsight;

CENsightAftermathShaderDatabase::CENsightAftermathShaderDatabase() {
}

CENsightAftermathShaderDatabase::~CENsightAftermathShaderDatabase() {
}

bool CENsightAftermathShaderDatabase::FindShaderBinary(const GFSDK_Aftermath_ShaderHash& ShaderHash,
	std::vector<uint8_t>& Shader) const {
}

bool CENsightAftermathShaderDatabase::FindShaderBinary(
	const GFSDK_Aftermath_ShaderInstructionsHash& ShaderInstructionsHash, std::vector<uint8_t>& Shader) const {
}

bool CENsightAftermathShaderDatabase::FindSourceShaderDebugData(const GFSDK_Aftermath_ShaderDebugName& ShaderDebugName,
	std::vector<uint8_t>& DebugData) const {
}

void CENsightAftermathShaderDatabase::AddShaderBinary(const char* FilePath) {
}

void CENsightAftermathShaderDatabase::AddSourceShaderDebugData(const char* FilePath, const char* FileName) {
}

bool CENsightAftermathShaderDatabase::ReadFile(const char* FileName, std::vector<uint8_t>& Data) {
}
