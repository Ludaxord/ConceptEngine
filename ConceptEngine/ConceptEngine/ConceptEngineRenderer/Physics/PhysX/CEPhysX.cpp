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
#include <extensions/PxShapeExt.h>
#include <foundation/PxVec4.h>
#include <pvd/PxPvd.h>
#include <pvd/PxPvdTransport.h>

#define MAX_NUM_ACTOR_SHAPES 128

physx::PxDefaultAllocator PhysXAllocator;
physx::PxDefaultErrorCallback PhysXErrorCallback;

physx::PxFoundation* PhysXFoundation = nullptr;
physx::PxPhysics* PhysXPhysics = nullptr;

physx::PxDefaultCpuDispatcher* PhysXDispatcher = nullptr;
physx::PxScene* PhysXScene = nullptr;

physx::PxControllerManager* PhysXControllerManager = nullptr;

physx::PxPvd* PhysXPvd = nullptr;

std::unordered_map<std::string, physx::PxRigidDynamic*> PxRigidDynamicMap;
std::unordered_map<std::string, physx::PxRigidStatic*> PxRigidStaticMap;
std::unordered_map<std::string, physx::PxController*> PxControllerMap;

CEPhysX::CEPhysX() {
}

CEPhysX::~CEPhysX() {
}

bool CEPhysX::Create() {
	PhysXFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, PhysXAllocator, PhysXErrorCallback);

	if (!PhysXFoundation) {
		CE_LOG_ERROR("[CEPhysX]: Failed to Create PhysX Foundation");
		return false;
	}

	PhysXPvd = physx::PxCreatePvd(*PhysXFoundation);
	if (!PhysXPvd) {
		CE_LOG_ERROR("[CEPhysX]: Failed to Create PhysX Pvd");
	}
	else {
		physx::PxPvdTransport* Transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		if (!PhysXPvd->connect(*Transport, physx::PxPvdInstrumentationFlag::eALL)) {
			CE_LOG_WARNING("[CEPhysX]: Failed to Connect to PhysX Pvd")
		}
	}


	PhysXPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *PhysXFoundation, physx::PxTolerancesScale(), true, PhysXPvd);
	if (!PhysXPhysics) {
		CE_LOG_ERROR("[CEPhysX]: Failed to create PhysX Physics")
		return false;
	}

	return true;
}

bool CEPhysX::CreateScene() {
	physx::PxSceneDesc SceneDesc(PhysXPhysics->getTolerancesScale());
	SceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	SceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	PhysXScene = PhysXPhysics->createScene(SceneDesc);
	if (!PhysXScene) {
		CE_LOG_ERROR("[CEPhysX]: Failed to Create PhysX Scene");
		return false;
	}

	PhysXControllerManager = PxCreateControllerManager(*PhysXScene);

	physx::PxPvdSceneClient* PvdClient = PhysXScene->getScenePvdClient();
	if (PvdClient) {
		PvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		PvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		PvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	return true;
}

void CEPhysX::Update(CETimestamp DeltaTime) {
	Accumulator += DeltaTime;
	if (Accumulator < StepSize) {
		return;
	}

	Accumulator -= StepSize;

	PhysXScene->simulate(StepSize.AsSeconds());
	PhysXScene->fetchResults(true);
}

std::string CEPhysX::CreatePXRigidStatic(void* pDesc) {
	std::string Name = Math::RandStr();
	while (HasPXRigidStatic(Name)) {
		Name = Math::RandStr();
	}

	PxRigidStaticDesc Desc = *static_cast<PxRigidStaticDesc*>(pDesc);

	physx::PxTransform Transform = physx::PxTransform(
		physx::PxVec3(Desc.Pos.x, Desc.Pos.y, Desc.Pos.z),
		physx::PxQuat(Desc.Quat.x, Desc.Quat.y, Desc.Quat.z, Desc.Quat.w)
	);

	physx::PxRigidStatic* Actor = PhysXPhysics->createRigidStatic(Transform);

	physx::PxMaterial* Material = PhysXPhysics->createMaterial(Desc.Material.x, Desc.Material.y, Desc.Material.z);

	physx::PxShape* Shape = nullptr;
	switch (Desc.PxGeometry) {

	case PxGeometryEnum::PxSphereEnum: {
		Shape = PhysXPhysics->createShape(physx::PxSphereGeometry(Desc.Scale.x), *Material);
	}
	break;
	case PxGeometryEnum::PxBoxEnum: {
		Shape = PhysXPhysics->createShape(physx::PxBoxGeometry(Desc.Scale.x, Desc.Scale.y, Desc.Scale.z), *Material);
	}
	break;
	case PxGeometryEnum::PxCapsuleEnum: {
		Shape = PhysXPhysics->createShape(physx::PxCapsuleGeometry(Desc.Scale.x, Desc.Scale.y), *Material);
	}
	break;
	case PxGeometryEnum::PxPlaneEnum: {
		Shape = PhysXPhysics->createShape(physx::PxPlaneGeometry(), *Material);
	}
	break;
	default: {
		CE_LOG_ERROR("[CEPhysX]: Wrong Geometry");
		CEDebug::DebugBreak();
	}
	}

	Actor->attachShape(*Shape);
	Shape->release();

	PhysXScene->addActor(*Actor);
	PxRigidStaticMap[Name] = Actor;

	return Name;
}

std::string CEPhysX::CreatePXRigidDynamic(void* pDesc) {
	std::string Name = Math::RandStr();
	while (HasPXRigidStatic(Name)) {
		Name = Math::RandStr();
	}

	PxRigidDynamicDesc Desc = *static_cast<PxRigidDynamicDesc*>(pDesc);

	physx::PxTransform Transform = physx::PxTransform(
		physx::PxVec3(Desc.Pos.x, Desc.Pos.y, Desc.Pos.z),
		physx::PxQuat(Desc.Quat.x, Desc.Quat.y, Desc.Quat.z, Desc.Quat.w)
	);

	physx::PxRigidDynamic* Actor = PhysXPhysics->createRigidDynamic(Transform);

	physx::PxMaterial* Material = PhysXPhysics->createMaterial(Desc.Material.x, Desc.Material.y, Desc.Material.z);

	physx::PxShape* Shape = nullptr;
	switch (Desc.PxGeometry) {

	case PxGeometryEnum::PxSphereEnum: {
		Shape = PhysXPhysics->createShape(physx::PxSphereGeometry(Desc.Scale.x), *Material);
	}
	break;
	case PxGeometryEnum::PxBoxEnum: {
		Shape = PhysXPhysics->createShape(physx::PxBoxGeometry(Desc.Scale.x, Desc.Scale.y, Desc.Scale.z), *Material);
	}
	break;
	case PxGeometryEnum::PxCapsuleEnum: {
		Shape = PhysXPhysics->createShape(physx::PxCapsuleGeometry(Desc.Scale.x, Desc.Scale.y), *Material);
	}
	break;
	case PxGeometryEnum::PxPlaneEnum: {
		Shape = PhysXPhysics->createShape(physx::PxPlaneGeometry(), *Material);
	}
	break;
	default: {
		CE_LOG_ERROR("[CEPhysX]: Wrong Geometry");
		CEDebug::DebugBreak();
	}
	}

	Actor->attachShape(*Shape);
	Shape->release();

	PhysXScene->addActor(*Actor);
	PxRigidDynamicMap[Name] = Actor;

	return Name;
}

void CEPhysX::AddForce(std::string Name, physx::PxVec3 Force) {
	if (!HasPXRigidDynamic(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxRigidDynamic does not exists");
		CEDebug::DebugBreak();
	}

	PxRigidDynamicMap[Name]->addForce(Force);
}

void CEPhysX::SetRigidDynamicLockFlag(std::string Name, CEPhysXAxis Axis, bool DynamicLockFlag) {

	if (!HasPXRigidDynamic(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxRigidDynamic does not exists");
		CEDebug::DebugBreak();
	}

	switch (Axis) {

	case CEPhysXAxis::Linear_X: {
		PxRigidDynamicMap[Name]->
			setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, DynamicLockFlag);
	}
	break;
	case CEPhysXAxis::Linear_Y: {
		PxRigidDynamicMap[Name]->
			setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, DynamicLockFlag);
	}
	break;
	case CEPhysXAxis::Linear_Z: {
		PxRigidDynamicMap[Name]->
			setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, DynamicLockFlag);
	}
	break;
	case CEPhysXAxis::Angular_X: {
		PxRigidDynamicMap[Name]->
			setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, DynamicLockFlag);
	}
	break;
	case CEPhysXAxis::Angular_Y: {
		PxRigidDynamicMap[Name]->
			setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, DynamicLockFlag);
	}
	break;
	case CEPhysXAxis::Angular_Z: {
		PxRigidDynamicMap[Name]->
			setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, DynamicLockFlag);
	}
	break;
	default: {
		CE_LOG_ERROR("[CEPhysX]: Wrong Axis")
		CEDebug::DebugBreak();
	};
	}

}

void CEPhysX::SetAngularDamping(std::string Name, float AngularDamping) {
	if (!HasPXRigidDynamic(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxRigidDynamic does not exists");
		CEDebug::DebugBreak();
	}

	PxRigidDynamicMap[Name]->setAngularDamping(AngularDamping);
}

void CEPhysX::SetLinearVelocity(std::string Name, physx::PxVec3 LinearVelocity) {
	if (!HasPXRigidDynamic(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxRigidDynamic does not exists");
		CEDebug::DebugBreak();
	}

	PxRigidDynamicMap[Name]->setLinearVelocity(LinearVelocity);
}

void CEPhysX::SetKinematicFlag(std::string Name, bool KinematicFlag) {
	if (!HasPXRigidDynamic(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxRigidDynamic does not exists");
		CEDebug::DebugBreak();
	}

	PxRigidDynamicMap[Name]->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, KinematicFlag);
}

void CEPhysX::SetKinematicTarget(std::string Name, physx::PxVec3& Position, physx::PxQuat& Quat) {
	if (!HasPXRigidDynamic(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxRigidDynamic does not exists");
		CEDebug::DebugBreak();
	}

	physx::PxTransform Target = physx::PxTransform(Position, Quat);
	PxRigidDynamicMap[Name]->setKinematicTarget(Target);
}

void CEPhysX::ReleasePXRigidStatic(std::string Name) {
	if (!HasPXRigidStatic(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxRigidStatic does not exists");
		CEDebug::DebugBreak();
	}
	else {
		PxRigidStaticMap[Name]->release();
	}

	PxRigidStaticMap.erase(Name);

}

void CEPhysX::ReleasePXRigidDynamic(std::string Name) {
	if (!HasPXRigidDynamic(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxRigidDynamic does not exists");
		CEDebug::DebugBreak();
	}
	else {
		PxRigidDynamicMap[Name]->release();
	}

	PxRigidDynamicMap.erase(Name);
}

std::string CEPhysX::AddCharacterController(void* pDesc) {
	std::string Name = Math::RandStr();
	while (HasPXController(Name)) {
		Name = Math::RandStr();
	}

	PxCapsuleControllerDesc* CapsuleControllerDesc = static_cast<PxCapsuleControllerDesc*>(pDesc);

	physx::PxCapsuleControllerDesc Desc;
	Desc.position = physx::PxExtendedVec3(
		CapsuleControllerDesc->Position.x,
		CapsuleControllerDesc->Position.y,
		CapsuleControllerDesc->Position.z
	);
	Desc.contactOffset = CapsuleControllerDesc->ContactOffset;
	Desc.stepOffset = CapsuleControllerDesc->StepOffset;
	Desc.slopeLimit = CapsuleControllerDesc->SlopeLimit;
	Desc.radius = CapsuleControllerDesc->Radius;
	Desc.height = CapsuleControllerDesc->Height;
	Desc.upDirection = physx::PxVec3(
		CapsuleControllerDesc->UpDirection.x,
		CapsuleControllerDesc->UpDirection.y,
		CapsuleControllerDesc->UpDirection.z
	);
	Desc.material = PhysXPhysics->createMaterial(0.0f, 0.0f, 0.0f);

	physx::PxController* Controller = PhysXControllerManager->createController(Desc);
	PxControllerMap[Name] = Controller;

	return Name;
}

void CEPhysX::ReleaseCharacterController(std::string& Name) {
	if (!HasPXController(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxController does not exists");
		CEDebug::DebugBreak();
	}
	else {
		PxControllerMap[Name]->release();
	}

	PxControllerMap.erase(Name);
}

int32 CEPhysX::MoveCharacterController(std::string& Name,
                                       const physx::PxVec3& Disp,
                                       float MinDist,
                                       float ElapsedTime) {
	if (!HasPXController(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxController does not exists");
		CEDebug::DebugBreak();
	}

	physx::PxControllerCollisionFlags Flag = PxControllerMap[Name]->move(Disp,
	                                                                     MinDist,
	                                                                     ElapsedTime,
	                                                                     physx::PxControllerFilters()
	);

	if (Flag == physx::PxControllerCollisionFlag::eCOLLISION_SIDES) {
		return 1;
	}
	else if (Flag == physx::PxControllerCollisionFlag::eCOLLISION_UP) {
		return 2;
	}
	else if (Flag == physx::PxControllerCollisionFlag::eCOLLISION_DOWN) {
		return 3;
	}

	return 0;
}

physx::PxExtendedVec3 CEPhysX::GetCharacterControllerTranslation(std::string& Name) {
	if (!HasPXController(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxController does not exists");
		CEDebug::DebugBreak();
	}

	return PxControllerMap[Name]->getPosition();
}

void CEPhysX::GetPXRigidDynamicTransform(std::string Name, physx::PxVec3& Position, physx::PxVec4& Quat) {
	if (!HasPXController(Name)) {
		CE_LOG_ERROR("[CEPhysX]: PxController does not exists");
		CEDebug::DebugBreak();
	}

	physx::PxRigidDynamic* RigidDynamic = PxRigidDynamicMap[Name];
	physx::PxShape* Shapes[MAX_NUM_ACTOR_SHAPES];
	RigidDynamic->getShapes(Shapes, 1);

	const physx::PxTransform Transform = physx::PxShapeExt::getGlobalPose(*Shapes[0], *RigidDynamic);

	Position.x = Transform.p.x;
	Position.y = Transform.p.y;
	Position.z = Transform.p.z;
	Quat.x = Transform.q.x;
	Quat.y = Transform.q.y;
	Quat.z = Transform.q.z;
	Quat.w = Transform.q.w;
}

void CEPhysX::Release() {
	PxRigidDynamicMap.clear();
	PxRigidStaticMap.clear();
	SafePhysXRelease(PhysXScene);
}

bool CEPhysX::HasPXRigidStatic(std::string& Name) {
	return PxRigidStaticMap.find(Name) != PxRigidStaticMap.end();
}

bool CEPhysX::HasPXRigidDynamic(std::string& Name) {
	return PxRigidDynamicMap.find(Name) != PxRigidDynamicMap.end();
}

bool CEPhysX::HasPXController(std::string& Name) {
	return PxControllerMap.find(Name) != PxControllerMap.end();
}
