#include "CEPhysX.h"

#include <PxFoundation.h>
#include <PxPhysics.h>
#include <PxPhysicsVersion.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <PxScene.h>
#include <characterkinematic/PxCapsuleController.h>
#include <characterkinematic/PxControllerManager.h>
#include <common/PxTolerancesScale.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxShapeExt.h>
#include <foundation/PxVec4.h>
#include <pvd/PxPvd.h>
#include <pvd/PxPvdTransport.h>

struct CEPhysXInternals {
	physx::PxFoundation* PhysXFoundation;
	physx::PxDefaultCpuDispatcher* PhysXCPUDispatcher;
	physx::PxPhysics* PhysXSDK;
	physx::PxDefaultAllocator Allocator;
	CEPhysXErrorCallback ErrorCallback;
	CEPhysXAssertHandler AssertHandler;
};

static CEPhysXInternals* PhysicsInternals;

void CEPhysXErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) {
	std::string ErrMessage = "";
	switch (code) {
	case physx::PxErrorCode::eNO_ERROR: ErrMessage = "No Error";
		break;
	case physx::PxErrorCode::eDEBUG_INFO: ErrMessage = "Info";
		break;
	case physx::PxErrorCode::eDEBUG_WARNING: ErrMessage = "Warning";
		break;
	case physx::PxErrorCode::eINVALID_PARAMETER: ErrMessage = "Invalid Parameter";
		break;
	case physx::PxErrorCode::eINVALID_OPERATION: ErrMessage = "Invalid Operation";
		break;
	case physx::PxErrorCode::eOUT_OF_MEMORY: ErrMessage = "Out Of Memory";
		break;
	case physx::PxErrorCode::eINTERNAL_ERROR: ErrMessage = "Internal Error";
		break;
	case physx::PxErrorCode::eABORT: ErrMessage = "Abort";
		break;
	case physx::PxErrorCode::ePERF_WARNING: ErrMessage = "Performance Warning";
		break;
	case physx::PxErrorCode::eMASK_ALL: ErrMessage = "Unknown Error";
		break;
	}

	switch (code) {
	case physx::PxErrorCode::eNO_ERROR:
	case physx::PxErrorCode::eDEBUG_INFO:
		CE_LOG_INFO("[CEPhysX]: "+ ErrMessage +": "+ message +" at "+ file +" ("+ std::to_string(line) +")");
		break;
	case physx::PxErrorCode::eDEBUG_WARNING:
	case physx::PxErrorCode::ePERF_WARNING:
		CE_LOG_WARNING("[CEPhysX]: "+ ErrMessage +": "+ message +" at "+ file +" ("+ std::to_string(line) +")");
		break;
	case physx::PxErrorCode::eINVALID_PARAMETER:
	case physx::PxErrorCode::eINVALID_OPERATION:
	case physx::PxErrorCode::eOUT_OF_MEMORY:
	case physx::PxErrorCode::eINTERNAL_ERROR:
		CE_LOG_ERROR("[CEPhysX]: "+ ErrMessage +": "+ message +" at "+ file +" ("+ std::to_string(line) +")");
		break;
	case physx::PxErrorCode::eABORT:
	case physx::PxErrorCode::eMASK_ALL:
		CE_LOG_ERROR("[CEPhysX]: "+ ErrMessage +": "+ message +" at "+ file +" ("+ std::to_string(line) +")");
		CEDebug::DebugBreak();
		break;
	}
}

void CEPhysXAssertHandler::operator()(const char* exp, const char* file, int line, bool& ignore) {
}

CEPhysX::CEPhysX() {
	PhysicsManager = new CEPhysXManager();
}

CEPhysX::~CEPhysX() {
}

bool CEPhysX::Create() {
	if (!CreateConfig()) {
		CE_LOG_ERROR("[CEPhysX]: Failed to Create PhysX Config");
		CEDebug::DebugBreak();
		return false;
	}

	if (!PhysicsManager->Create()) {
		CE_LOG_ERROR("[CEPhysX]: Failed to Create PhysXManager");
		CEDebug::DebugBreak();
		return false;
	}
	return true;
}

void CEPhysX::Release() {
}

void CEPhysX::ReleaseScene() {
}

bool CEPhysX::CreateConfig() {
	PhysicsInternals = new CEPhysXInternals();
	PhysicsInternals->PhysXFoundation = PxCreateFoundation(
		PX_PHYSICS_VERSION, PhysicsInternals->Allocator, PhysicsInternals->ErrorCallback);
	if (!PhysicsInternals->PhysXFoundation) {
		CE_LOG_ERROR("[CEPhysX]: PxCreateFoundation Failed...")
		return false;
	}

	physx::PxTolerancesScale Scale = physx::PxTolerancesScale();
	Scale.length = 1.0f;
	Scale.speed = 9.81f;

	if (!CreateDebugger()) {
		CE_LOG_ERROR("[CEPhysX]: CreateDebugger Failed...")
		return false;
	}

	static bool TrackMemoryAllocations = true;

	PhysicsInternals->PhysXSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *PhysicsInternals->PhysXFoundation, Scale,
	                                             TrackMemoryAllocations, PhysicsDebugger->GetDebugger());
	if (!PhysicsInternals->PhysXSDK) {
		CE_LOG_ERROR("[CEPhysX]: PxCreatePhysics Failed...")
		return false;
	}

	if (!PxInitExtensions(*PhysicsInternals->PhysXSDK, PhysicsDebugger->GetDebugger())) {
		CE_LOG_ERROR("[CEPhysX]: PxInitExtensions Failed...")
		return false;
	}

	PhysicsInternals->PhysXCPUDispatcher = physx::PxDefaultCpuDispatcherCreate(1);

	if (!CreateCookingFactory()) {
		CE_LOG_ERROR("[CEPhysX]: CreateCookingFactory Failed...")
		return false;
	}

	physx::PxSetAssertHandler(PhysicsInternals->AssertHandler);

	return true;
}

void CEPhysX::CreateActors(Scene* Scene) {
}

CEPhysicsActor* CEPhysX::CreateActor(Actor* InActor) {
	return {};
}

bool CEPhysX::CreateDebugger() {
	PhysicsDebugger = new CEPhysXDebugger();

	return true;
}

bool CEPhysX::CreateCookingFactory() {
	CookingFactory = new CECookingFactory();

	return true;
}

bool CEPhysX::CreateScene() {

	return true;
}


bool CEPhysX::CreatePhysXInternals() {

}
