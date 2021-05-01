#include "CEPhysX.h"

#include "../../Tools/CEUtils.h"
using namespace ConceptEngineFramework::Physics::PhysX;

CEPhysX::CEPhysX() {
}

CEPhysX::~CEPhysX() {
}

void CEPhysX::InitPhysics() {
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);

	m_pvd = PxCreatePvd(*m_foundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), true, m_pvd);
	PxInitExtensions(*m_physics, m_pvd);

	CE_LOG("Initialize PhysX Engine");
	spdlog::info("Initialize PhysX Engine");
}

void CEPhysX::CreateScene() {
}

void CEPhysX::CleanUpScene() {
}

void CEPhysX::CreateStaticObject(void* pDesc) {
}

void CEPhysX::CreateDynamicObject(void* pDesc) {
}

void CEPhysX::AddCharacterController(void* pDesc) {
}

void CEPhysX::RemoveCharacterController(const std::string& name) {
}

void CEPhysX::MoveCharacterController(const std::string& name,
                                      float& dispX,
                                      float& dispY,
                                      float& dispZ,
                                      float minDist,
                                      float elapsedTime) {
}

void CEPhysX::Update(float delta) {
}

void CEPhysX::CleanUpPhysics() {
}

void CEPhysX::AddForce(std::string name, float x, float y, float z) {
}
