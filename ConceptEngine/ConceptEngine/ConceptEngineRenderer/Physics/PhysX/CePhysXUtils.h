#pragma once
#include <PxRigidDynamic.h>
#include <foundation/PxMat44.h>
#include <foundation/PxVec3.h>
#include <foundation/PxVec4.h>
#include <cooking/PxCooking.h>

#include "Scene/Components/CETransformComponent.h"

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

inline CECookingResult FromPhysXCookingResult(physx::PxConvexMeshCookingResult::Enum CookingResult) {
	switch (CookingResult) {
	case physx::PxConvexMeshCookingResult::eSUCCESS: return CECookingResult::Success;
	case physx::PxConvexMeshCookingResult::eZERO_AREA_TEST_FAILED: return CECookingResult::ZeroAreaTestFailed;
	case physx::PxConvexMeshCookingResult::ePOLYGONS_LIMIT_REACHED: return CECookingResult::PolygonLimitReached;
	case physx::PxConvexMeshCookingResult::eFAILURE: return CECookingResult::Failure;
	}

	return CECookingResult::Failure;
}

inline CECookingResult FromPhysXCookingResult(physx::PxTriangleMeshCookingResult::Enum CookingResult) {
	switch (CookingResult) {
	case physx::PxTriangleMeshCookingResult::eSUCCESS: return CECookingResult::Success;
	case physx::PxTriangleMeshCookingResult::eLARGE_TRIANGLE: return CECookingResult::LargeTriangle;
	case physx::PxTriangleMeshCookingResult::eFAILURE: return CECookingResult::Failure;
	}

	return CECookingResult::Failure;
}

inline physx::PxTransform ToPhysXTransform(const DirectX::XMFLOAT3& Translation, const DirectX::XMFLOAT3 Rotation) {
	return physx::PxTransform(ToPhysXVector(Translation),
	                          ToPhysXQuat(DirectX::XMFLOAT4(Rotation.x, Rotation.y, Rotation.z, 0.0f))
	);
}

inline physx::PxTransform ToPhysXTransform(const DirectX::XMFLOAT4X4& Transform) {
	DirectX::XMVECTOR Translation;
	DirectX::XMVECTOR Rotation;
	DirectX::XMVECTOR Scale;
	const auto MatTransform = DirectX::XMLoadFloat4x4(&Transform);
	DirectX::XMMatrixDecompose(&Scale, &Rotation, &Translation, MatTransform);
	DirectX::XMFLOAT4 FRotation;
	DirectX::XMFLOAT3 FTranslation;
	XMStoreFloat4(&FRotation, Rotation);
	XMStoreFloat3(&FTranslation, Translation);
	physx::PxQuat Rot = ToPhysXQuat(FRotation);
	physx::PxVec3 Tran = ToPhysXVector(FTranslation);
	return physx::PxTransform(Tran, Rot);

}

inline physx::PxTransform ToPhysXTransform(const CETransformComponent& Transform) {
	physx::PxQuat Rot = ToPhysXQuat(XMFLOAT4(Transform.Rotation.x, Transform.Rotation.y, Transform.Rotation.z, 0.0f));
	physx::PxVec3 Tran = ToPhysXVector(Transform.Translation);
	return physx::PxTransform(Tran, Rot);
}

inline physx::PxRigidDynamicLockFlag::Enum ToPhysXActorLockFlag(CEActorLockFlag Flag) {
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
