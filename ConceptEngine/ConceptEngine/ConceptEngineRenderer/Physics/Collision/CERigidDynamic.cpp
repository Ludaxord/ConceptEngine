#include "CERigidDynamic.h"

#include "../Boot/CECore.h"
#include "../Graphics/D3D12/Managers/CEDX12MeshManager.h"
#include "../Graphics/Generic/Managers/CEManagers.h"

CERigidDynamic::CERigidDynamic(
	std::string& InParentName,
	const CERigidTransform& InParentTransform,
	const CERigidTransform& InLocalTransform
): CERigid(InParentName, InParentTransform, InLocalTransform) {
}

CERigidDynamic::~CERigidDynamic() {
	CERigid::~CERigid();
}

void CERigidDynamic::CreatePhysicsMesh() {
	//TODO: Check if Mesh with RigidBodyName Does Not Exists...
	if (RigidBodyName != "") {
		MeshData MeshData;
		switch (PxGeometry) {
		case PxGeometryEnum::PxSphereEnum: {
			MeshData = CastMeshManager()->CreateSphere(20, Scale.x);
		}
		break;
		case PxGeometryEnum::PxBoxEnum: {
			MeshData = CastMeshManager()->CreateCube(Scale.x * 2, Scale.y * 2, Scale.z * 2);
		}
		break;
		case PxGeometryEnum::PxCapsuleEnum: {
		}
		break;
		case PxGeometryEnum::PxPlaneEnum: {
			MeshData = CastMeshManager()->CreatePlane(Scale.x, Scale.y);
		}
		break;
		default: {

		};
		}

		Mesh = Mesh::Make(MeshData);
	}
}

void CERigidDynamic::AddRigidDynamic() {
	CERigid::AddRigidDynamic();

	PxRigidDynamicDesc Desc;

	Desc.Pos.x = WorldTransform.Translation.x;
	Desc.Pos.y = WorldTransform.Translation.y;
	Desc.Pos.z = WorldTransform.Translation.z;

	Desc.Quat.x = WorldTransform.Quaternion.x;
	Desc.Quat.y = WorldTransform.Quaternion.y;
	Desc.Quat.z = WorldTransform.Quaternion.z;
	Desc.Quat.w = WorldTransform.Quaternion.w;

	Desc.Material.x = PxMaterial.x;
	Desc.Material.y = PxMaterial.y;
	Desc.Material.z = PxMaterial.z;

	Desc.PxGeometry = PxGeometry;

	Desc.Scale.x = Scale.x;
	Desc.Scale.y = Scale.y;
	Desc.Scale.z = Scale.z;
	Desc.Scale.w = Scale.w;

	Desc.Density = Density;

	Name = CECore::GetPhysics()->CreatePXRigidDynamic(&Desc);
	CECore::GetPhysics()->SetKinematicFlag(Name, IsKinematic);

	CreatePhysicsMesh();
}

void CERigidDynamic::Release() {
	CERigid::Release();
}

void CERigidDynamic::Create() {
	ParentTransform.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	LocalTransform.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMMATRIX LocalMatrix = RigidTransformToMatrix(LocalTransform);
	XMMATRIX WorldMatrix = RigidTransformToMatrix(ParentTransform);

	XMVECTOR WorldPositionVector;
	XMVECTOR WorldQuaternionVector;
	XMVECTOR WorldScaleVector;

	XMMatrixDecompose(&WorldScaleVector, &WorldQuaternionVector, &WorldPositionVector, WorldMatrix);

	XMStoreFloat3(&WorldTransform.Translation, WorldPositionVector);
	XMStoreFloat4(&WorldTransform.Quaternion, WorldQuaternionVector);

	XMVECTOR MatrixDeterminantLocalMatrix = XMMatrixDeterminant(LocalMatrix);
	XMStoreFloat4x4(&LocalMatrixReverse, XMMatrixInverse(&MatrixDeterminantLocalMatrix, LocalMatrix));

	AddRigidDynamic();
}

void CERigidDynamic::CreatePhysicsComponent(Actor* InOwningActor) {
	CERigidTransform MeshTransform = WorldTransform;
	if (PxGeometry == PxGeometryEnum::PxCapsuleEnum) {
		MeshTransform = RotateRigidTransformLocal(WorldTransform, WorldTransform.GetForward(), -XM_PIDIV2);
	}

	MaterialProperties MatProperties;
	PhysicsComponent = DBG_NEW CEPhysicsComponent(InOwningActor);
	PhysicsComponent->MeshTransform = MeshTransform;
	PhysicsComponent->Material = MakeShared<Material>(MatProperties);
	XMStoreFloat4x4(&PhysicsComponent->TextureTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	PhysicsComponent->FillMode = EFillMode::WireFrame;
	PhysicsComponent->ReceiveShadow = false;
}

void CERigidDynamic::Create(Actor* InOwningActor) {
	Create();
	CreatePhysicsComponent(InOwningActor);
}

void CERigidDynamic::Update(CERigidTransform& InParentTransform) {

	// CE_LOG_VERBOSE("[CERigidDynamic]: Update Parent Name: "+ ParentName +" \n ParentTransform Scale{X:" +
	// 	std::to_string(InParentTransform.Scale.x) + " Y:" + std::to_string(InParentTransform.Scale.y) + " Z:" + std::
	// 	to_string(InParentTransform.Scale.z) + "} Translation{X:" + std::to_string(InParentTransform.Translation.x) +
	// 	"Y:" + std::to_string(InParentTransform.Translation.y) + " Z:" + std::to_string(InParentTransform.Translation.z)
	// 	+ "} Quaternion{X:" + std::to_string(InParentTransform.Quaternion.x) +
	// 	"Y:" + std::to_string(InParentTransform.Quaternion.y) + " Z:" + std::to_string(InParentTransform.Quaternion.z) +
	// 	" W:" + std::to_string(InParentTransform.Quaternion.w) + "}")

	if (IsKinematic) {
		PushTransform(InParentTransform);
	}
	else {
		PullTransform();

		ParentTransform.Translation = InParentTransform.Translation;
		ParentTransform.Quaternion = InParentTransform.Quaternion;
	}


	CERigidTransform MeshTransform = WorldTransform;
	if (PxGeometry == PxGeometryEnum::PxCapsuleEnum) {
		MeshTransform = RotateRigidTransformLocal(WorldTransform, WorldTransform.GetForward(), -XM_PIDIV2);
	}

	//TODO: Update Mesh...
}

void CERigidDynamic::AddForce(DirectX::XMFLOAT3 Force) {
	dynamic_cast<CEPhysX*>(CECore::GetPhysics())->AddForce(Name, physx::PxVec3(Force.x, Force.y, Force.z));
}

void CERigidDynamic::SetRigidDynamicLockFlag(int Axis, bool DynamicLockFlag) {
	dynamic_cast<CEPhysX*>(CECore::GetPhysics())->SetRigidDynamicLockFlag(
		Name, static_cast<CEPhysXAxis>(Axis), DynamicLockFlag);
}

void CERigidDynamic::SetAngularDamping(float AngularDamping) {
	dynamic_cast<CEPhysX*>(CECore::GetPhysics())->SetAngularDamping(Name, AngularDamping);
}

void CERigidDynamic::SetLinearVelocity(DirectX::XMFLOAT3 Velocity) {
	dynamic_cast<CEPhysX*>(CECore::GetPhysics())->SetLinearVelocity(
		Name, physx::PxVec3(Velocity.x, Velocity.y, Velocity.z));
}

void CERigidDynamic::PushTransform(const CERigidTransform& InParentTransform) {
	XMMATRIX LocalMatrix = RigidTransformToMatrix(LocalTransform);
	XMMATRIX WorldMatrix = LocalMatrix * RigidTransformToMatrix(InParentTransform);
	XMVECTOR WorldPositionVector;
	XMVECTOR WorldQuaternionVector;
	XMVECTOR WorldScaleVector;

	XMMatrixDecompose(&WorldScaleVector, &WorldQuaternionVector, &WorldPositionVector, WorldMatrix);

	XMStoreFloat3(&WorldTransform.Translation, WorldPositionVector);
	XMStoreFloat4(&WorldTransform.Quaternion, WorldQuaternionVector);

	physx::PxVec3 Pos = physx::PxVec3(WorldTransform.Translation.x, WorldTransform.Translation.y,
	                                  WorldTransform.Translation.z);
	physx::PxQuat Quat = physx::PxQuat(WorldTransform.Quaternion.x, WorldTransform.Quaternion.y,
	                                   WorldTransform.Quaternion.z, WorldTransform.Quaternion.w);
	dynamic_cast<CEPhysX*>(CECore::GetPhysics())->SetKinematicTarget(Name, Pos, Quat);

}

void CERigidDynamic::PullTransform() {
	physx::PxVec3 Pos;
	physx::PxQuat Quat;
	dynamic_cast<CEPhysX*>(CECore::GetPhysics())->GetPXRigidDynamicTransform(Name, Pos, Quat);

	WorldTransform.Translation = XMFLOAT3(Pos.x, Pos.y, Pos.z);
	WorldTransform.Quaternion = XMFLOAT4(Quat.x, Quat.y, Quat.z, Quat.w);

	XMMATRIX NewWorld = XMLoadFloat4x4(&LocalMatrixReverse) * RigidTransformToMatrix(WorldTransform);

	XMVECTOR NewWorldPositionVector;
	XMVECTOR NewWorldQuaternionVector;
	XMVECTOR NewWorldScaleVector;
	XMMatrixDecompose(&NewWorldScaleVector, &NewWorldPositionVector, &NewWorldQuaternionVector, NewWorld);

	XMStoreFloat3(&ParentTransform.Translation, NewWorldPositionVector);
	XMStoreFloat4(&ParentTransform.Quaternion, NewWorldQuaternionVector);
}
