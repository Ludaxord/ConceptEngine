#pragma once
#include <pvd/PxPvd.h>


class CEPhysXDebugger {
public:
	bool Create();
	physx::PxPvd* GetDebugger() {
		
	}

	void StopDebugging();
	void Release();
	bool IsDebugging();
	void StartDebugging(const std::string& Path, bool LiveDebugging);
};
