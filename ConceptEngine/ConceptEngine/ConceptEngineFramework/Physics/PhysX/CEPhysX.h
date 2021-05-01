#pragma once

#include "../CEPhysics.h"


#pragma comment(lib, "PhysX_static_64.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "PhysXCommon_static_64.lib")
#pragma comment(lib, "PhysXCooking_static_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXFoundation_static_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysXVehicle_static_64.lib")

#include "PxPhysicsAPI.h"

#ifndef PHYSX_SNIPPET_PVD_H
#define PHYSX_SNIPPET_PVD_H
#define PVD_HOST "127.0.0.1"
#endif

namespace ConceptEngineFramework::Physics::PhysX {


	class CEPhysX : public CEPhysics {
	public:
		CEPhysX();
		~CEPhysX() override;

		void InitPhysics() override;
		void CreateScene() override;
		void CleanUpScene() override;
		void CreateStaticObject(void* pDesc) override;
		void CreateDynamicObject(void* pDesc) override;
		void AddCharacterController(void* pDesc) override;
		void RemoveCharacterController(const std::string& name) override;
		void MoveCharacterController(const std::string& name, float& dispX, float& dispY, float& dispZ, float minDist,
		                             float elapsedTime) override;
		void Update(float delta) override;
		void CleanUpPhysics() override;
	protected:
		void AddForce(std::string name, float x, float y, float z) override;

	private:
		physx::PxDefaultAllocator m_allocator;
		physx::PxDefaultErrorCallback m_errorCallback;

		physx::PxFoundation* m_foundation = nullptr;
		physx::PxPhysics* m_physics = nullptr;

		physx::PxDefaultCpuDispatcher* m_dispatcher = nullptr;
		physx::PxScene* m_scene = nullptr;

		physx::PxControllerManager* m_controllerManager = nullptr;

		physx::PxPvd* m_pvd = nullptr;
	};
}
