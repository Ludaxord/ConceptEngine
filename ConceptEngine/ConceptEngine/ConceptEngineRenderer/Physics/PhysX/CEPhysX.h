#pragma once
#include "Physics/CEPhysics.h"
#include <foundation/PxVec3.h>

class CEPhysX : public CEPhysics {
public:
	CEPhysX();
	~CEPhysX();

	bool Create() override;
	bool CreateScene() override;

	void Update(CETimestamp DeltaTime) override;

	std::string CreatePXRigidStatic(void* Desc);
	std::string CreatePXRigidDynamic(void* Desc);

	void AddForce(std::string Name, physx::PxVec3 Force);

	void SetRigidDynamicLockFlag(std::string Name, int Axis, bool DynamicLockFlag);
	void SetAngularDamping(std::string Name, float AngularDamping);
	void SetLinearVelocity(std::string Name, physx::PxVec3 LinearVelocity);
	void SetKinematicFlag(std::string Name, bool KinematicFlag);
	void SetKinematicTarget(std::string Name, physx::PxVec3& Position, physx::PxVec4& Quat);

	void ReleasePXRigidStatic(std::string Name);
	void ReleasePXRigidDynamic(std::string Name);

	std::string AddCharacterController(void* Desc);
	void ReleaseCharacterController(const std::string& Name);
	int32 MoveCharacterController(const std::string& Name, const physx::PxVec3& Disp, float MinDist, float ElapsedTime);
	physx::PxVec3 GetCharacterControllerTranslation(const std::string& Name);

	void GetPXRigidDynamicTransform(std::string Name, physx::PxVec3& Position, physx::PxVec4& Quat);

	void Release();

private:
	bool HasPXRigidStatic(std::string& Name);
	bool HasPXRigidDynamic(std::string& Name);
	bool HasPXController(std::string& Name);
};
