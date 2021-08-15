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

#include "Scene/Components/CERigidBodyComponent.h"

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
	CE_LOG_ERROR("[CEPhysX]: "+ std::string(file) +":"+ std::to_string(line) +" - " + exp);
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

	if (!CreatePhysXInternals()) {
		CE_LOG_ERROR("[CEPhysX]: Failed to Create PhysX Internals");
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

	CookingFactory->Release();
	PhysicsInternals->PhysXCPUDispatcher->release();
	PhysicsInternals->PhysXCPUDispatcher = nullptr;

	PxCloseExtensions();

	PhysicsDebugger->StopDebugging();

	PhysicsInternals->PhysXSDK->release();
	PhysicsInternals->PhysXSDK = nullptr;

	PhysicsDebugger->Release();

	PhysicsInternals->PhysXFoundation->release();
	PhysicsInternals->PhysXFoundation = nullptr;

	delete PhysicsInternals;
	PhysicsInternals = nullptr;

}

void CEPhysX::ReleaseScene() {
}

bool CEPhysX::CreateConfig() {

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

physx::PxFoundation& CEPhysX::GetFoundation() {
	return *PhysicsInternals->PhysXFoundation;
}

physx::PxPhysics& CEPhysX::GetPhysXSDK() {
	return *PhysicsInternals->PhysXSDK;
}

physx::PxCpuDispatcher& CEPhysX::GetCPUDispatcher() {
	return *PhysicsInternals->PhysXCPUDispatcher;
}

physx::PxDefaultAllocator& CEPhysX::GetAllocator() {
	return PhysicsInternals->Allocator;
}

physx::PxFilterFlags CEPhysX::FilterShader(physx::PxFilterObjectAttributes Attr0, physx::PxFilterData FilterData0,
                                           physx::PxFilterObjectAttributes Attr1, physx::PxFilterData FilterData1,
                                           physx::PxPairFlags& PairFlags,
                                           const void* ConstantBlock, physx::PxU32 ConstantBlockSize) {
	if (physx::PxFilterObjectIsTrigger(Attr0) || physx::PxFilterObjectIsTrigger(Attr1)) {
		PairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	PairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	if (FilterData0.word2 == (uint32)CERigidBodyComponent::CollisionDetectionType::Continuous ||
		FilterData1.word2 == (uint32)CERigidBodyComponent::CollisionDetectionType::Continuous) {
		PairFlags |= physx::PxPairFlag::eDETECT_DISCRETE_CONTACT;
		PairFlags |= physx::PxPairFlag::eDETECT_CCD_CONTACT;
	}

	if ((FilterData0.word0 & FilterData1.word1) || (FilterData1.word0 & FilterData0.word1)) {

		PairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		PairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
		return physx::PxFilterFlag::eDEFAULT;
	}

	return physx::PxFilterFlag::eSUPPRESS;
}

physx::PxBroadPhaseType::Enum CEPhysX::GetPhysXBroadPhaseType(PhysicsBroadPhaseType Type) {
	switch (Type) {
	case PhysicsBroadPhaseType::SweepAndPrune: return physx::PxBroadPhaseType::eSAP;
	case PhysicsBroadPhaseType::MultiBoxPrune: return physx::PxBroadPhaseType::eMBP;
	case PhysicsBroadPhaseType::AutomaticBoxPrune: return physx::PxBroadPhaseType::eABP;
	}

	return physx::PxBroadPhaseType::eABP;
}

physx::PxFrictionType::Enum CEPhysX::GetPhysXFrictionType(PhysicsFrictionType Type) {
	switch (Type) {
	case PhysicsFrictionType::Patch: return physx::PxFrictionType::ePATCH;
	case PhysicsFrictionType::OneDirectional: return physx::PxFrictionType::eONE_DIRECTIONAL;
	case PhysicsFrictionType::TwoDirectional: return physx::PxFrictionType::eTWO_DIRECTIONAL;
	}

	return physx::PxFrictionType::ePATCH;
}

bool CEPhysX::CreateScene() {

	return true;
}


bool CEPhysX::CreatePhysXInternals() {
	PhysicsInternals = new CEPhysXInternals();
	PhysicsInternals->PhysXFoundation = PxCreateFoundation(
		PX_PHYSICS_VERSION, PhysicsInternals->Allocator, PhysicsInternals->ErrorCallback);
	if (!PhysicsInternals->PhysXFoundation) {
		CE_LOG_ERROR("[CEPhysX]: PxCreateFoundation Failed...")
		return false;
	}

	physx::PxTolerancesScale Scale;
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
