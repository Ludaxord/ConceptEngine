#pragma once
#include <characterkinematic/PxControllerManager.h>
#include <characterkinematic/PxExtended.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultErrorCallback.h>

#include "../Physics/CEPhysics.h"
#include <foundation/PxVec3.h>
#include <foundation/PxVec4.h>
#include <pvd/PxPvd.h>

//TODO: change PhysX Implementation to more complex
enum class CEPhysXAxis {
	Linear_X,
	Linear_Y,
	Linear_Z,
	Angular_X,
	Angular_Y,
	Angular_Z
};


enum class PxGeometryEnum { PxSphereEnum, PxBoxEnum, PxCapsuleEnum, PxPlaneEnum };

struct PxRigidDynamicDesc {
	physx::PxVec3 Pos;
	physx::PxVec4 Quat;

	physx::PxVec3 Material;

	PxGeometryEnum PxGeometry;
	physx::PxVec4 Scale;

	float Density;
};

struct PxRigidStaticDesc {
	physx::PxVec3 Pos;
	physx::PxVec4 Quat;

	physx::PxVec3 Material;

	PxGeometryEnum PxGeometry;
	physx::PxVec4 Scale;
};

struct PxCapsuleControllerDesc {
	physx::PxVec3 Position;
	float ContactOffset;
	float StepOffset;
	float SlopeLimit;
	float Radius;
	float Height;
	physx::PxVec3 UpDirection;
};

class CEPhysX : public CEPhysics {
public:
	CEPhysX();
	~CEPhysX();

	bool Create() override;
	bool CreateScene() override;

	void Update(CETimestamp DeltaTime) override;
	void Release() override;

	std::string CreatePXRigidStatic(void* Desc) override;
	std::string CreatePXRigidDynamic(void* Desc) override;

	void AddForce(std::string Name, physx::PxVec3 Force);

	void SetRigidDynamicLockFlag(std::string Name, CEPhysXAxis Axis, bool DynamicLockFlag);
	void SetAngularDamping(std::string Name, float AngularDamping);
	void SetLinearVelocity(std::string Name, physx::PxVec3 LinearVelocity);
	void SetKinematicFlag(std::string Name, bool KinematicFlag) override;
	void SetKinematicTarget(std::string Name, physx::PxVec3& Position, physx::PxQuat& Quat);

	void ReleasePXRigidStatic(std::string Name);
	void ReleasePXRigidDynamic(std::string Name);

	std::string AddCharacterController(void* Desc);
	void ReleaseCharacterController(std::string& Name);
	int32 MoveCharacterController(std::string& Name, const physx::PxVec3& Disp, float MinDist, float ElapsedTime);
	physx::PxExtendedVec3 GetCharacterControllerTranslation(std::string& Name);

	void GetPXRigidDynamicTransform(std::string Name, physx::PxVec3& Position, physx::PxQuat& Quat);


private:
	bool HasPXRigidStatic(std::string& Name);
	bool HasPXRigidDynamic(std::string& Name);
	bool HasPXController(std::string& Name);

	CETimestamp Accumulator = 0.0f;
	CETimestamp StepSize = 1.0f / 60.0f;
};


extern physx::PxDefaultAllocator PhysXAllocator;
extern physx::PxDefaultErrorCallback PhysXErrorCallback;

extern physx::PxFoundation* PhysXFoundation;
extern physx::PxPhysics* PhysXPhysics;

extern physx::PxDefaultCpuDispatcher* PhysXDispatcher;
extern physx::PxScene* PhysXScene;

extern physx::PxControllerManager* PhysXControllerManager;

extern physx::PxPvd* PhysXPvd;
