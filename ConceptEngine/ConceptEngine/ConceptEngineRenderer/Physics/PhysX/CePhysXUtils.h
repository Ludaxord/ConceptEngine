#pragma once
#include <PxRigidDynamic.h>
#include <foundation/PxMat44.h>
#include <foundation/PxVec3.h>
#include <foundation/PxVec4.h>

enum class CECookingResult {
	Success,
	ZeroAreaTestFailed,
	PolygonLimitReached,
	LargeTriangle,
	Failure
};

enum class CEForceMode: uint8 {
	Force = 0,
	Impulse,
	VelocityChange,
	Acceleration
};

enum class CEActorLockFlag {
	PositionX = BIT(0),
	PositionY = BIT(1),
	PositionZ = BIT(2),
	RotationX = BIT(3),
	RotationY = BIT(4),
	RotationZ = BIT(5)
};

inline const physx::PxVec3& ToPhysXVector(const DirectX::XMFLOAT3& Vector) {
	return *(physx::PxVec3*)&Vector;
}

inline const physx::PxVec4& ToPhysXVector(const DirectX::XMFLOAT4& Vector) {
	return *(physx::PxVec4*)&Vector;
}

inline physx::PxMat44 ToPhysXMatrix(const DirectX::XMFLOAT4X4& Matrix) {
	return *(physx::PxMat44*)&Matrix;
}

inline physx::PxQuat ToPhysXQuat(const DirectX::XMFLOAT4& Quat) {
	return physx::PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
}

inline DirectX::XMFLOAT3 FromPhysXVector(const physx::PxVec3& Vector) {
	return *(DirectX::XMFLOAT3*)&Vector;
}

inline DirectX::XMFLOAT4 FromPhysXVector(const physx::PxVec4& Vector) {
	return *(DirectX::XMFLOAT4*)&Vector;
}

inline DirectX::XMFLOAT4 FromPhysXQuat(const physx::PxQuat& Quat) {
	return XMFLOAT4(Quat.x, Quat.y, Quat.z, Quat.w);
}

inline physx::PxTransform ToPhysXTransform(const DirectX::XMFLOAT3& Translation, const DirectX::XMFLOAT3 Rotation) {
	return physx::PxTransform(ToPhysXVector(Translation),
	                          ToPhysXQuat(DirectX::XMFLOAT4(Rotation.x, Rotation.y, Rotation.z, 0.0f))
	);
}

physx::PxRigidDynamicLockFlag::Enum ToPhysXActorLockFlag(CEActorLockFlag Flag) {
	switch (Flag) {
	case CEActorLockFlag::PositionX:
		return physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
	case CEActorLockFlag::PositionY:
		return physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
	case CEActorLockFlag::PositionZ:
		return physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
	case CEActorLockFlag::RotationX:
		return physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
	case CEActorLockFlag::RotationY:
		return physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
	case CEActorLockFlag::RotationZ:
		return physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
	}

	Assert(false);
	return physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
}
