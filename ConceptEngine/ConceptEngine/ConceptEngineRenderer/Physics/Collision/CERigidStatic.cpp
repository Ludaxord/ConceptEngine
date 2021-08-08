#include "CERigidStatic.h"

#include "Boot/CECore.h"
#include "Graphics/D3D12/Managers/CEDX12MeshManager.h"
#include "Graphics/Generic/Managers/CEManagers.h"
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Mesh.h"

CERigidStatic::CERigidStatic(
	std::string& InParentName,
	const CERigidTransform& InParentTransform,
	const CERigidTransform& InLocalTransform
): CERigid(InParentName, InParentTransform, InLocalTransform) {

}


CERigidStatic::~CERigidStatic() {
	CERigid::~CERigid();
}

void CERigidStatic::AddRigidStatic() {
	CERigid::AddRigidStatic();

	PxRigidStaticDesc Desc;

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

	Name = CECore::GetPhysics()->CreatePXRigidStatic(&Desc);

	CreatePhysicsMesh();
}

void CERigidStatic::Release() {
	CERigid::Release();
}

void CERigidStatic::Create() {
	ParentTransform.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	LocalTransform.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	XMMATRIX LocalMatrix = RigidTransformToMatrix(LocalTransform);
	XMMATRIX ParentMatrix = LocalMatrix * RigidTransformToMatrix(ParentTransform);

	XMVECTOR WorldPositionVector;
	XMVECTOR WorldQuaternionVector;
	XMVECTOR WorldScaleVector;
	XMMatrixDecompose(&WorldScaleVector, &WorldQuaternionVector, &WorldPositionVector, ParentMatrix);

	XMStoreFloat3(&WorldTransform.Translation, WorldPositionVector);
	XMStoreFloat4(&WorldTransform.Quaternion, WorldQuaternionVector);

	AddRigidStatic();
}

void CERigidStatic::CreatePhysicsMesh() {

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

void CERigidStatic::CreatePhysicsComponent() {
	CERigidTransform MeshTransform = WorldTransform;
	if (PxGeometry == PxGeometryEnum::PxCapsuleEnum) {
		MeshTransform = RotateRigidTransformLocal(WorldTransform, WorldTransform.GetForward(), -XM_PIDIV2);
	}

	MaterialProperties MatProperties;
	PhysicsComponent = DBG_NEW CEPhysicsComponent();
	PhysicsComponent->MeshTransform = MeshTransform;
	PhysicsComponent->Material = MakeShared<Material>(MatProperties);
	XMStoreFloat4x4(&PhysicsComponent->TextureTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	PhysicsComponent->FillMode = EFillMode::WireFrame;
	PhysicsComponent->ReceiveShadow = false;
}
