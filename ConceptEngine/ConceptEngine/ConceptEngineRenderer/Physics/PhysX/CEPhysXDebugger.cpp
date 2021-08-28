#include "CEPhysXDebugger.h"

#include <pvd/PxPvdTransport.h>

#include "CEPhysX.h"

struct CEPhysXData {
	physx::PxPvd* Debugger;
	physx::PxPvdTransport* Transport;
};

static CEPhysXData* PhysXData = nullptr;

bool CEPhysXDebugger::Create() {
	PhysXData = DBG_NEW CEPhysXData();

	PhysXData->Debugger = physx::PxCreatePvd(CEPhysX::GetFoundation());

	if (!PhysXData->Debugger) {
		return false;
	}

	return true;
}

physx::PxPvd* CEPhysXDebugger::GetDebugger() {
	return PhysXData->Debugger;
}

//TODO: Implement
void CEPhysXDebugger::StopDebugging() {
	if (!PhysXData->Debugger->isConnected())
		return;

	PhysXData->Debugger->disconnect();
	PhysXData->Transport->release();
}

void CEPhysXDebugger::Release() {
	PhysXData->Debugger->release();
	delete PhysXData;
	PhysXData = nullptr;
}

bool CEPhysXDebugger::IsDebugging() {
	return PhysXData->Debugger->isConnected();
}

void CEPhysXDebugger::StartDebugging(const std::string& Path, bool LiveDebugging) {
	StopDebugging();

	if (!LiveDebugging) {
		PhysXData->Transport = physx::PxDefaultPvdFileTransportCreate((Path + ".pxd2").c_str());
		PhysXData->Debugger->connect(*PhysXData->Transport, physx::PxPvdInstrumentationFlag::eALL);
	}
	else {
		PhysXData->Transport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 1000);
		PhysXData->Debugger->connect(*PhysXData->Transport, physx::PxPvdInstrumentationFlag::eALL);
	}
}
