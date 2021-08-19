#pragma once
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
