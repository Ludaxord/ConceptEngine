#include "CEPhysXShapes.h"

#include "CEPhysX.h"
#include <PxPhysics.h>
#include <extensions/PxRigidActorExt.h>

#include "Physics/CEPhysicsActor.h"
#include "Physics/CEPhysicsManagers.h"
#include "Physics/CEPhysicsMaterial.h"
#include "Scene/Components/CETransformComponent.h"

#include "Rendering/Resources/Mesh.h"


void CEColliderShape::SetMaterial(CEPhysicsMaterial* PhysicsMaterial) {
	if (!PhysicsMaterial)
		PhysicsMaterial = DBG_NEW CEPhysicsMaterial(0.6f, 0.6f, 0.6f);

	if (Material != nullptr)
		Material->release();

	Material = CEPhysX::GetPhysXSDK().createMaterial(PhysicsMaterial->StaticFriction,
	                                                 PhysicsMaterial->DynamicFriction,
	                                                 PhysicsMaterial->Bounciness);
}

CEBoxColliderShape::CEBoxColliderShape(CEColliderBoxComponent& Component,
                                       const CEPhysicsActor& PActor,
                                       Actor* OwningActor,
                                       const DirectX::XMFLOAT3& Offset): CEColliderShape(CECollisionType::Box),
                                                                         MainComponent(Component) {
	SetMaterial(MainComponent.Material.Get());

	DirectX::XMFLOAT3 ColliderSize = OwningActor->GetComponentOfType<CETransformComponent>()->Scale * Component.Size;
	physx::PxBoxGeometry Geometry = physx::PxBoxGeometry(ColliderSize.x / 2.0f,
	                                                     ColliderSize.y / 2.0f,
	                                                     ColliderSize.z / 2.0f);
	Shape = physx::PxRigidActorExt::createExclusiveShape(*PActor.GetPhysXActor(), Geometry, *Material);
	Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !Component.IsTrigger);
	Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, Component.IsTrigger);
	Shape->setLocalPose(ToPhysXTransform(Offset + Component.Offset, XMFLOAT3(0.0f, 0.0f, 0.0f)));
}

CEBoxColliderShape::~CEBoxColliderShape() {
	CEColliderShape::~CEColliderShape();
}

CEBoxColliderShape* CEBoxColliderShape::Instance(CEColliderBoxComponent& Component,
                                                 const CEPhysicsActor& PActor,
                                                 Actor* OwningActor,
                                                 const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CEBoxColliderShape(Component, PActor, OwningActor, Offset);
}

const DirectX::XMFLOAT3& CEBoxColliderShape::GetOffset() const {
	return MainComponent.Offset;
}

void CEBoxColliderShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
	Shape->setLocalPose(ToPhysXTransform(Offset, XMFLOAT3(0.0f, 0.0f, 0.0f)));
	MainComponent.Offset = Offset;
}

bool CEBoxColliderShape::IsTrigger() const {
	return MainComponent.IsTrigger;
}

void CEBoxColliderShape::SetTrigger(bool IsTrigger) {
	Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !IsTrigger);
	Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, IsTrigger);
	MainComponent.IsTrigger = IsTrigger;
}

void CEBoxColliderShape::SetFilter(const physx::PxFilterData& FilterData) {
	Shape->setSimulationFilterData(FilterData);
}

void CEBoxColliderShape::DetachFromActor(physx::PxRigidActor* Actor) {
	Actor->detachShape(*Shape);
}

CESphereColliderShape::CESphereColliderShape(CEColliderSphereComponent& Component,
                                             const CEPhysicsActor& PActor,
                                             Actor* OwningActor,
                                             const DirectX::XMFLOAT3& Offset) :
	CEColliderShape(CECollisionType::Sphere),
	MainComponent(Component) {
	SetMaterial(MainComponent.Material.Get());

	auto& ActorScale = OwningActor->GetComponentOfType<CETransformComponent>()->Scale;
	float LargestComponent = Math::Max(ActorScale.x, Math::Max(ActorScale.y, ActorScale.z));

	physx::PxSphereGeometry Geometry = physx::PxSphereGeometry(LargestComponent * Component.Radius);
	Shape = physx::PxRigidActorExt::createExclusiveShape(*PActor.GetPhysXActor(), Geometry, *Material);
	Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !Component.IsTrigger);
	Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, Component.IsTrigger);
}

CESphereColliderShape::~CESphereColliderShape() {
	CEColliderShape::~CEColliderShape();
}

CESphereColliderShape* CESphereColliderShape::Instance(CEColliderSphereComponent& Component,
                                                       const CEPhysicsActor& PActor,
                                                       Actor* OwningActor,
                                                       const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CESphereColliderShape(Component, PActor, OwningActor, Offset);
}

const DirectX::XMFLOAT3& CESphereColliderShape::GetOffset() const {
	return MainComponent.Offset;
}

void CESphereColliderShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
	Shape->setLocalPose(ToPhysXTransform(Offset, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)));
	MainComponent.Offset = Offset;
}

bool CESphereColliderShape::IsTrigger() const {
	return MainComponent.IsTrigger;
}

void CESphereColliderShape::SetTrigger(bool IsTrigger) {
	Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !IsTrigger);
	Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, IsTrigger);
	MainComponent.IsTrigger = IsTrigger;
}

void CESphereColliderShape::SetFilter(const physx::PxFilterData& filterData) {
	Shape->setSimulationFilterData(filterData);
}

void CESphereColliderShape::DetachFromActor(physx::PxRigidActor* Actor) {
	Actor->detachShape(*Shape);
}

//TODO: Implement..
CECapsuleColliderShape::CECapsuleColliderShape(CEColliderCapsuleComponent& Component,
                                               const CEPhysicsActor& PActor,
                                               Actor* OwningActor,
                                               const DirectX::XMFLOAT3& Offset) : CEColliderShape(
		CECollisionType::Capsule),
	MainComponent(Component) {
	SetMaterial(MainComponent.Material.Get());

	auto& ActorScale = OwningActor->GetComponentOfType<CETransformComponent>()->Scale;
	float RadiusScale = Math::Max(ActorScale.x, Math::Max(ActorScale.y, ActorScale.z));

	physx::PxCapsuleGeometry Geometry = physx::PxCapsuleGeometry(Component.Radius * RadiusScale,
	                                                             (Component.Height / 2.0f) * ActorScale.y);
	Shape = physx::PxRigidActorExt::createExclusiveShape(*PActor.GetPhysXActor(), Geometry, *Material);
	Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !Component.IsTrigger);
	Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, Component.IsTrigger);
	Shape->setLocalPose(ToPhysXTransform(Offset + Component.Offset, XMFLOAT3(0.0f, 0.0f, Math::HALF_PI)));
}

CECapsuleColliderShape::~CECapsuleColliderShape() {
	CEColliderShape::~CEColliderShape();
}

CECapsuleColliderShape* CECapsuleColliderShape::Instance(CEColliderCapsuleComponent& Component,
                                                         const CEPhysicsActor& PActor, Actor* OwningActor,
                                                         const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CECapsuleColliderShape(Component, PActor, OwningActor, Offset);
}

const DirectX::XMFLOAT3& CECapsuleColliderShape::GetOffset() const {
	return MainComponent.Offset;
}

void CECapsuleColliderShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
	Shape->setLocalPose(ToPhysXTransform(Offset, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)));
	MainComponent.Offset = Offset;
}

bool CECapsuleColliderShape::IsTrigger() const {
	return MainComponent.IsTrigger;
}

void CECapsuleColliderShape::SetTrigger(bool IsTrigger) {
	Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !IsTrigger);
	Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, IsTrigger);
	MainComponent.IsTrigger = IsTrigger;
}

void CECapsuleColliderShape::SetFilter(const physx::PxFilterData& filterData) {
	Shape->setSimulationFilterData(filterData);
}

void CECapsuleColliderShape::DetachFromActor(physx::PxRigidActor* Actor) {
	Actor->detachShape(*Shape);
}

CEConvexMeshShape::CEConvexMeshShape(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                     Actor* OwningActor, const DirectX::XMFLOAT3& Offset) :
	CEColliderShape(CECollisionType::ConvexMesh), MainComponent(Component) {
	Assert(Component.IsConvex);

	SetMaterial(MainComponent.Material.Get());

	CEArray<CEMeshColliderData> CookedData;
	CookedData.Reserve(Component.CollisionMesh->VertexCount);
	CECookingResult Result = CookingFactory->CookMesh(Component, CookedData, false);
	Assert(CookedData.Size() > 0);

	if (Result != CECookingResult::Success)
		return;

	for (auto& ColliderData : CookedData) {
		XMVECTOR SubmeshTranslation;
		XMVECTOR SubmeshRotation;
		XMVECTOR SubmeshScale;

		DirectX::XMMatrixDecompose(&SubmeshScale, &SubmeshRotation, &SubmeshTranslation,
		                           XMLoadFloat4x4(&ColliderData.Transform));

		physx::PxDefaultMemoryInputData Input(ColliderData.Data, ColliderData.Size);
		physx::PxConvexMesh* ConvexMesh = CEPhysX::GetPhysXSDK().createConvexMesh(Input);
		XMFLOAT3 FloatSubmeshScale;
		XMStoreFloat3(&FloatSubmeshScale, SubmeshScale);

		physx::PxConvexMeshGeometry ConvexGeometry = physx::PxConvexMeshGeometry(
			ConvexMesh, physx::PxMeshScale(
				ToPhysXVector(FloatSubmeshScale * OwningActor->GetComponentOfType<CETransformComponent>()->Scale)
			)
		);
		ConvexGeometry.meshFlags = physx::PxConvexMeshGeometryFlag::eTIGHT_BOUNDS;

		physx::PxShape* Shape = CEPhysX::GetPhysXSDK().createShape(ConvexGeometry, *Material, true);
		Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !Component.IsTrigger);
		Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, Component.IsTrigger);
		Shape->setLocalPose(ToPhysXTransform(ColliderData.Transform));

		PActor.GetPhysXActor()->attachShape(*Shape);

		Shapes.PushBack(Shape);

		Shape->release();
		ConvexMesh->release();

		delete[] ColliderData.Data;
	}

	CookedData.Clear();
}

CEConvexMeshShape::~CEConvexMeshShape() {
	CEColliderShape::~CEColliderShape();
}

//TODO: Implement..
CEConvexMeshShape* CEConvexMeshShape::Instance(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                               Actor* OwningActor, const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CEConvexMeshShape(Component, PActor, OwningActor, Offset);
}

const DirectX::XMFLOAT3& CEConvexMeshShape::GetOffset() const {
	return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void CEConvexMeshShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

bool CEConvexMeshShape::IsTrigger() const {
	return MainComponent.IsTrigger;
}

void CEConvexMeshShape::SetTrigger(bool IsTrigger) {
	for (auto Shape : Shapes) {
		Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !IsTrigger);
		Shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, IsTrigger);
	}

	MainComponent.IsTrigger = IsTrigger;
}

void CEConvexMeshShape::SetFilter(const physx::PxFilterData& filterData) {
	for (auto Shape : Shapes)
		Shape->setSimulationFilterData(filterData);
}

void CEConvexMeshShape::DetachFromActor(physx::PxRigidActor* Actor) {
	for (auto Shape : Shapes)
		Actor->detachShape(*Shape);

	Shapes.Clear();
}

//TODO: Implement..
CETriangleMeshShape::CETriangleMeshShape(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                         Actor* OwningActor, const DirectX::XMFLOAT3& Offset) : CEColliderShape(
	CECollisionType::TriangleMesh), MainComponent(Component) {
	Assert(!Component.IsConvex);

	SetMaterial(MainComponent.Material.Get());

}

//TODO: Implement..
CETriangleMeshShape::~CETriangleMeshShape() {
	CEColliderShape::~CEColliderShape();
}

//TODO: Implement..
CETriangleMeshShape* CETriangleMeshShape::Instance(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                                   Actor* OwningActor, const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CETriangleMeshShape(Component, PActor, OwningActor, Offset);
}

const DirectX::XMFLOAT3& CETriangleMeshShape::GetOffset() const {
	return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void CETriangleMeshShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

bool CETriangleMeshShape::IsTrigger() const {
	return MainComponent.IsTrigger;
}

//TODO: Implement..
void CETriangleMeshShape::SetTrigger(bool IsTrigger) {
}

//TODO: Implement..
void CETriangleMeshShape::SetFilter(const physx::PxFilterData& filterData) {
}

//TODO: Implement..
void CETriangleMeshShape::DetachFromActor(physx::PxRigidActor* Actor) {
}
