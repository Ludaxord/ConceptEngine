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

	Name = CECore::GetPhysics()->CreatePXRigidStatic(&Desc);
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

void CERigidDynamic::Update(CERigidTransform& ParentTransform) {
	if (IsKinematic) {
		PushTransform(ParentTransform);
	}
	else {
		PullTransform();

		ParentTransform.Translation = ParentTransform.Translation;
		ParentTransform.Quaternion = ParentTransform.Quaternion;
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

//TODO: Implement
void CERigidDynamic::PushTransform(const CERigidTransform& ParentTransform) {
}

//TODO: Implement
void CERigidDynamic::PullTransform() {
}
