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
	physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	m_scene = m_physics->createScene(sceneDesc);

	m_controllerManager = PxCreateControllerManager(*m_scene);

	physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	CE_LOG("Create PhysX Scene");
	spdlog::info("Create PhysX Scene");
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
	m_accumulator += delta;
	if (m_accumulator < m_stepSize) {
		return;
	}

	m_accumulator -= m_stepSize;

	m_scene->simulate(m_stepSize);
	m_scene->fetchResults(true);
}

void CEPhysX::CleanUpPhysics() {
}

void CEPhysX::AddForce(std::string name, float x, float y, float z) {
}
