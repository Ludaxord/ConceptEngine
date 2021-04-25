#pragma once
#include <Windows.h> // For HRESULT
#include <fcntl.h>

#include <iostream>
#include <comdef.h>
#include <spdlog/spdlog.h>

struct CESystemInfo {
	float RamSize;
	std::string CPUName;
	float CPUCores;
};

inline void CELog(std::string s) {
	std::cout << s << std::endl;
}

#define CE_LOG(log) CELog(log);

inline std::string FormatError(HRESULT hr) {
	std::string strError;
	char strHR[64] = {};

	sprintf_s(strHR, "0x%08X", hr);

	strError = "Failed to initialize COM. Error code = " + std::string(strHR);
	return strError;
}

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		auto ssHr = FormatError(hr);
		spdlog::error(ssHr);
		Sleep(1000);
		throw std::exception(ssHr.c_str());
	}
}

inline CESystemInfo GetEngineSystemInfo() {
	CESystemInfo systemInfo;
	int CPUInfo[4] = {-1};
	unsigned nExIds, i = 0;
	char CPUBrandString[0x40];
	// Get the information associated with each extended ID.
	__cpuid(CPUInfo, 0x80000000);
	nExIds = CPUInfo[0];
	for (i = 0x80000000; i <= nExIds; ++i) {
		__cpuid(CPUInfo, i);
		// Interpret CPU brand string
		if (i == 0x80000002)
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000003)
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000004)
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
	}
	//string includes manufacturer, model and clockspeed
	systemInfo.CPUName = CPUBrandString;

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	systemInfo.CPUCores = sysInfo.dwNumberOfProcessors;

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	systemInfo.RamSize = (statex.ullTotalPhys / 1024) / 1024;
	return systemInfo;
}
