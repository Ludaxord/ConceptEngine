#include "CENsightAftermathGPUCrashTracker.h"

#include <fstream>

using namespace ConceptEngine::Core::Debug::Nsight;

CENsightAftermathGPUCrashTracker::CENsightAftermathGPUCrashTracker(): Initialized(false),
                                                                      Mutex(),
                                                                      ShaderDebugInfo(),
                                                                      ShaderDatabase() {
}

CENsightAftermathGPUCrashTracker::~CENsightAftermathGPUCrashTracker() {
	if (Initialized) {
		GFSDK_Aftermath_DisableGpuCrashDumps();
	}
}

void CENsightAftermathGPUCrashTracker::Create() {
	AFTERMATH_CHECK_ERROR(
		GFSDK_Aftermath_EnableGpuCrashDumps(GFSDK_Aftermath_Version_API,
			GFSDK_Aftermath_GpuCrashDumpWatchedApiFlags_DX,
			GFSDK_Aftermath_GpuCrashDumpFeatureFlags_DeferDebugInfoCallbacks,
			GPUCrashDumpCallback,
			ShaderDebugInfoCallback,
			CrashDumpDescriptionCallback,
			this
		));

	Initialized = true;
}

void CENsightAftermathGPUCrashTracker::OnCrashDump(const void* GpuCrashDump, const uint32_t GpuCrashDumpSize) {
	std::lock_guard<std::mutex> lock(Mutex);

	WriteGpuCrashDumpToFile(GpuCrashDump, GpuCrashDumpSize);
}

void CENsightAftermathGPUCrashTracker::OnShaderDebugInfo(const void* ShaderDebugInfoPtr,
                                                         const uint32_t ShaderDebugInfoSize) {
	std::lock_guard<std::mutex> lock(Mutex);

	GFSDK_Aftermath_ShaderDebugInfoIdentifier Identifier = {};
	AFTERMATH_CHECK_ERROR(
		GFSDK_Aftermath_GetShaderDebugInfoIdentifier(
			GFSDK_Aftermath_Version_API,
			ShaderDebugInfoPtr,
			ShaderDebugInfoSize,
			&Identifier
		)
	);

	std::vector<uint8_t> data((uint8_t*)ShaderDebugInfoPtr, (uint8_t*)ShaderDebugInfoPtr + ShaderDebugInfoSize);
	ShaderDebugInfo[Identifier].swap(data);

	WriteShaderDebugInformationToFile(Identifier, ShaderDebugInfoPtr, ShaderDebugInfoSize);
}

void CENsightAftermathGPUCrashTracker::OnDescription(PFN_GFSDK_Aftermath_AddGpuCrashDumpDescription AddDescription) {
	AddDescription(GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationName, "ConceptEngine");
	AddDescription(GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationVersion, "v1.0");
	AddDescription(GFSDK_Aftermath_GpuCrashDumpDescriptionKey_UserDefined, "GPU Error: ");
	AddDescription(GFSDK_Aftermath_GpuCrashDumpDescriptionKey_UserDefined + 1, "Check data: ");
}

void CENsightAftermathGPUCrashTracker::WriteGpuCrashDumpToFile(const void* GpuCrashDump,
                                                               const uint32_t GpuCrashDumpSize) {
	GFSDK_Aftermath_GpuCrashDump_Decoder Decoder = {};
	AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_GpuCrashDump_CreateDecoder(
		GFSDK_Aftermath_Version_API,
		GpuCrashDump,
		GpuCrashDumpSize,
		&Decoder
	));

	GFSDK_Aftermath_GpuCrashDump_BaseInfo BaseInfo = {};
	AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_GpuCrashDump_GetBaseInfo(Decoder, &BaseInfo));

	uint32_t ApplicationNameLength = 0;
	AFTERMATH_CHECK_ERROR(
		GFSDK_Aftermath_GpuCrashDump_GetDescriptionSize(Decoder,
			GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationName,
			&ApplicationNameLength)
	);

	std::vector<char> ApplicationName(ApplicationNameLength, '\0');

	AFTERMATH_CHECK_ERROR(
		GFSDK_Aftermath_GpuCrashDump_GetDescription(
			Decoder,
			GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationName,
			uint32_t(ApplicationName.size()), ApplicationName.data())
	);

	static int count = 0;
	const std::string BaseFileName = std::string(ApplicationName.data());
	+ "-" + std::to_string(BaseInfo.pid) + "-" + std::to_string(++count);

	//TODO: Check if is possible to make reader of Nvidia Nsight Dump files...
	const std::string CrashDumpFileName = BaseFileName + ".nv-gpudmp";
	std::ofstream DumpFile(CrashDumpFileName, std::ios::out | std::ios::binary);
	if (DumpFile) {
		DumpFile.write((const char*)GpuCrashDump, GpuCrashDumpSize);
		DumpFile.close();
	}

	//TODO: Or make JSON Formatter inside Editor...
	uint32_t JsonSize = 0;
	AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_GpuCrashDump_GenerateJSON(
		Decoder,
		GFSDK_Aftermath_GpuCrashDumpDecoderFlags_ALL_INFO,
		GFSDK_Aftermath_GpuCrashDumpFormatterFlags_NONE,
		ShaderDebugInfoLookupCallback,
		ShaderLookupCallback,
		ShaderInstructionsLookupCallback,
		ShaderSourceDebugInfoLookupCallback,
		this,
		&JsonSize
	));

	std::vector<char> Json(JsonSize);
	AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_GpuCrashDump_GetJSON(
		Decoder,
		uint32_t(Json.size()),
		Json.data()
	));

	const std::string JsonFileName = CrashDumpFileName + ".json";
	std::ofstream JsonFile(JsonFileName, std::ios::out | std::ios::binary);
	if (JsonFile) {
		JsonFile.write(Json.data(), Json.size());
		JsonFile.close();
	}

	AFTERMATH_CHECK_ERROR(GFSDK_Aftermath_GpuCrashDump_DestroyDecoder(Decoder));
}

void CENsightAftermathGPUCrashTracker::WriteShaderDebugInformationToFile(
	GFSDK_Aftermath_ShaderDebugInfoIdentifier Identifier,
	const void* ShaderDebugInfo,
	const uint32_t ShaderDebugInfoSize) {
	const std::string FilePath = "Shader-" + std::to_string(Identifier) + ".nvdbg";

	std::ofstream f(FilePath, std::ios::out | std::ios::binary);
	if (f) {
		f.write((const char*)ShaderDebugInfo, ShaderDebugInfoSize);
	}
}

void CENsightAftermathGPUCrashTracker::OnShaderDebugInfoLookup(
	const GFSDK_Aftermath_ShaderDebugInfoIdentifier& Identifier, PFN_GFSDK_Aftermath_SetData SetShaderDebugInfo) const {
	auto debugInfo = ShaderDebugInfo.find(Identifier);
	if (debugInfo == ShaderDebugInfo.end()) {
		return;
	}

	SetShaderDebugInfo(debugInfo->second.data(), uint32_t(debugInfo->second.size()));
}

void CENsightAftermathGPUCrashTracker::OnShaderLookup(const GFSDK_Aftermath_ShaderHash& ShaderHash,
                                                      PFN_GFSDK_Aftermath_SetData SetShaderBinary) const {
	std::vector<uint8_t> ShaderBinary;
	if (!ShaderDatabase.FindShaderBinary(ShaderHash, ShaderBinary)) {
		return;
	}

	SetShaderBinary(ShaderBinary.data(), uint32_t(ShaderBinary.size()));
}

void CENsightAftermathGPUCrashTracker::OnShaderInstructionsLookup(
	const GFSDK_Aftermath_ShaderInstructionsHash& ShaderInstructionsHash,
	PFN_GFSDK_Aftermath_SetData SetShaderBinary) const {
	std::vector<uint8_t> ShaderBinary;
	if (!ShaderDatabase.FindShaderBinary(ShaderInstructionsHash, ShaderBinary)) {
		return;
	}

	SetShaderBinary(ShaderBinary.data(), uint32_t(ShaderBinary.size()));
}

void CENsightAftermathGPUCrashTracker::OnShaderSourceDebugInfoLookup(
	const GFSDK_Aftermath_ShaderDebugName& ShaderDebugName, PFN_GFSDK_Aftermath_SetData SetShaderBinary) const {
	std::vector<uint8_t> SourceDebugInfo;
	if (ShaderDatabase.FindSourceShaderDebugData(ShaderDebugName, SourceDebugInfo)) {
		return;
	}

	SetShaderBinary(SourceDebugInfo.data(), uint32_t(SourceDebugInfo.size()));
}

void CENsightAftermathGPUCrashTracker::GPUCrashDumpCallback(const void* GpuCrashDump, const uint32_t GpuCrashDumpSize,
                                                            void* UserData) {
	CENsightAftermathGPUCrashTracker* GpuCrashTracker = reinterpret_cast<CENsightAftermathGPUCrashTracker*>(UserData);
	GpuCrashTracker->OnCrashDump(GpuCrashDump, GpuCrashDumpSize);
}

void CENsightAftermathGPUCrashTracker::ShaderDebugInfoCallback(const void* ShaderDebugInfo,
                                                               const uint32_t ShaderDebugInfoSize,
                                                               void* UserData) {
	CENsightAftermathGPUCrashTracker* GpuCrashTracker = reinterpret_cast<CENsightAftermathGPUCrashTracker*>(UserData);
	GpuCrashTracker->OnShaderDebugInfo(ShaderDebugInfo, ShaderDebugInfoSize);
}

void CENsightAftermathGPUCrashTracker::CrashDumpDescriptionCallback(
	PFN_GFSDK_Aftermath_AddGpuCrashDumpDescription AddDescription,
	void* UserData) {
	CENsightAftermathGPUCrashTracker* GpuCrashTracker = reinterpret_cast<CENsightAftermathGPUCrashTracker*>(UserData);
	GpuCrashTracker->OnDescription(AddDescription);
}

void CENsightAftermathGPUCrashTracker::ShaderDebugInfoLookupCallback(
	const GFSDK_Aftermath_ShaderDebugInfoIdentifier* Identifier,
	PFN_GFSDK_Aftermath_SetData SetShaderDebugInfo,
	void* UserData) {
	CENsightAftermathGPUCrashTracker* GpuCrashTracker = reinterpret_cast<CENsightAftermathGPUCrashTracker*>(UserData);
	GpuCrashTracker->OnShaderDebugInfoLookup(*Identifier, SetShaderDebugInfo);
}

void CENsightAftermathGPUCrashTracker::ShaderLookupCallback(const GFSDK_Aftermath_ShaderHash* ShaderHash,
                                                            PFN_GFSDK_Aftermath_SetData SetShaderBinary,
                                                            void* UserData) {
	CENsightAftermathGPUCrashTracker* GpuCrashTracker = reinterpret_cast<CENsightAftermathGPUCrashTracker*>(UserData);
	GpuCrashTracker->OnShaderLookup(*ShaderHash, SetShaderBinary);
}

void CENsightAftermathGPUCrashTracker::ShaderInstructionsLookupCallback(
	const GFSDK_Aftermath_ShaderInstructionsHash* ShaderInstructionsHash,
	PFN_GFSDK_Aftermath_SetData SetShaderBinary,
	void* UserData) {
	CENsightAftermathGPUCrashTracker* GpuCrashTracker = reinterpret_cast<CENsightAftermathGPUCrashTracker*>(UserData);
	GpuCrashTracker->OnShaderInstructionsLookup(*ShaderInstructionsHash, SetShaderBinary);
}

void CENsightAftermathGPUCrashTracker::ShaderSourceDebugInfoLookupCallback(
	const GFSDK_Aftermath_ShaderDebugName* ShaderDebugName,
	PFN_GFSDK_Aftermath_SetData SetShaderBinary,
	void* UserData) {
	CENsightAftermathGPUCrashTracker* GpuCrashTracker = reinterpret_cast<CENsightAftermathGPUCrashTracker*>(UserData);
	GpuCrashTracker->OnShaderSourceDebugInfoLookup(*ShaderDebugName, SetShaderBinary);
}
