#pragma once
#include <map>
#include <mutex>
#include <vector>

#include "CENsightAftermathHelpers.h"
#include "CENsightAftermathShaderDatabase.h"

namespace ConceptEngine::Core::Debug::Nsight {
	//TODO: Create different versions for Vulkan && DX12 when Vulkan will be implemented into engine...
	class CENsightAftermathGPUCrashTracker {
	public:
		CENsightAftermathGPUCrashTracker();
		~CENsightAftermathGPUCrashTracker();

		void Create();

	private:
		void OnCrashDump(const void* GpuCrashDump, const uint32_t GpuCrashDumpSize);

		void OnShaderDebugInfo(const void* ShaderDebugInfo, const uint32_t ShaderDebugInfoSize);

		void OnDescription(PFN_GFSDK_Aftermath_AddGpuCrashDumpDescription AddDescription);

		void WriteGpuCrashDumpToFile(const void* GpuCrashDump, const uint32_t GpuCrashDumpSize);

		void WriteShaderDebugInformationToFile(GFSDK_Aftermath_ShaderDebugInfoIdentifier Identifier,
		                                       const void* ShaderDebugInfo,
		                                       const uint32_t ShaderDebugInfoSize);

		void OnShaderDebugInfoLookup(const GFSDK_Aftermath_ShaderDebugInfoIdentifier& Identifier,
		                             PFN_GFSDK_Aftermath_SetData SetShaderDebugInfo) const;

		void OnShaderLookup(const GFSDK_Aftermath_ShaderHash& ShaderHash,
		                    PFN_GFSDK_Aftermath_SetData SetShaderBinary) const;

		void OnShaderInstructionsLookup(const GFSDK_Aftermath_ShaderInstructionsHash& ShaderInstructionsHash,
		                                PFN_GFSDK_Aftermath_SetData SetShaderBinary) const;

		void OnShaderSourceDebugInfoLookup(const GFSDK_Aftermath_ShaderDebugName& ShaderDebugName,
		                                   PFN_GFSDK_Aftermath_SetData SetShaderBinary) const;

		static void GPUCrashDumpCallback(const void* GpuCrashDump, const uint32_t GpuCrashDumpSize, void* UserData);

		static void ShaderDebugInfoCallback(const void* ShaderDebugInfo,
		                                    const uint32_t ShaderDebugInfoSize,
		                                    void* UserData);

		static void CrashDumpDescriptionCallback(PFN_GFSDK_Aftermath_AddGpuCrashDumpDescription AddDescription,
		                                         void* UserData);

		static void ShaderDebugInfoLookupCallback(const GFSDK_Aftermath_ShaderDebugInfoIdentifier* Identifier,
		                                          PFN_GFSDK_Aftermath_SetData SetShaderDebugInfo, void* UserData);

		static void ShaderLookupCallback(const GFSDK_Aftermath_ShaderHash* ShaderHash,
		                                 PFN_GFSDK_Aftermath_SetData SetShaderBinary, void* UserData);

		static void ShaderInstructionsLookupCallback(
			const GFSDK_Aftermath_ShaderInstructionsHash* ShaderInstructionsHash,
			PFN_GFSDK_Aftermath_SetData SetShaderBinary, void* UserData);

		static void ShaderSourceDebugInfoLookupCallback(const GFSDK_Aftermath_ShaderDebugName* ShaderDebugName,
		                                                PFN_GFSDK_Aftermath_SetData SetShaderBinary, void* UserData);

		bool Initialized;

		mutable std::mutex Mutex;

		std::map<GFSDK_Aftermath_ShaderDebugInfoIdentifier, std::vector<uint8_t>> ShaderDebugInfo;

		CENsightAftermathShaderDatabase ShaderDatabase;
	};
}
