#include "CEPhysXShapes.h"

#include "CEPhysX.h"
#include <PxPhysics.h>
#include <extensions/PxRigidActorExt.h>

#include "Physics/CEPhysicsActor.h"
#include "Physics/CEPhysicsMaterial.h"
#include "Scene/Components/CETransformComponent.h"

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
                                                                         Component(Component) {
	SetMaterial(Component.Material.Get());

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
}

void CEBoxColliderShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

bool CEBoxColliderShape::IsTrigger() const {
}

void CEBoxColliderShape::SetTrigger(bool IsTrigger) {
}

void CEBoxColliderShape::SetFilter(const physx::PxFilterData& filterData) {
}

void CEBoxColliderShape::DetachFromActor(physx::PxRigidActor* Actor) {
}

CESphereColliderShape::CESphereColliderShape(CEColliderSphereComponent& Component,
                                             const CEPhysicsActor& PActor,
                                             Actor* OwningActor,
                                             const DirectX::XMFLOAT3& Offset) :
	CEColliderShape(CECollisionType::Sphere),
	Component(Component) {
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
}

void CESphereColliderShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

bool CESphereColliderShape::IsTrigger() const {
}

void CESphereColliderShape::SetTrigger(bool IsTrigger) {
}

void CESphereColliderShape::SetFilter(const physx::PxFilterData& filterData) {
}

void CESphereColliderShape::DetachFromActor(physx::PxRigidActor* Actor) {
}

CECapsuleColliderShape::CECapsuleColliderShape(CEColliderCapsuleComponent& Component,
                                               const CEPhysicsActor& PActor,
                                               Actor* OwningActor,
                                               const DirectX::XMFLOAT3& Offset) : CEColliderShape(
		CECollisionType::Capsule),
	Component(Component) {
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
}

void CECapsuleColliderShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

bool CECapsuleColliderShape::IsTrigger() const {
}

void CECapsuleColliderShape::SetTrigger(bool IsTrigger) {
}

void CECapsuleColliderShape::SetFilter(const physx::PxFilterData& filterData) {
}

void CECapsuleColliderShape::DetachFromActor(physx::PxRigidActor* Actor) {
}

CEConvexMeshShape::CEConvexMeshShape(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                     Actor* OwningActor, const DirectX::XMFLOAT3& Offset) :
	CEColliderShape(CECollisionType::ConvexMesh), Component(Component) {
}

CEConvexMeshShape::~CEConvexMeshShape() {
	CEColliderShape::~CEColliderShape();
}

CEConvexMeshShape* CEConvexMeshShape::Instance(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                               Actor* OwningActor, const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CEConvexMeshShape(Component, PActor, OwningActor, Offset);
}

const DirectX::XMFLOAT3& CEConvexMeshShape::GetOffset() const {
}

void CEConvexMeshShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

bool CEConvexMeshShape::IsTrigger() const {
}

void CEConvexMeshShape::SetTrigger(bool IsTrigger) {
}

void CEConvexMeshShape::SetFilter(const physx::PxFilterData& filterData) {
}

void CEConvexMeshShape::DetachFromActor(physx::PxRigidActor* Actor) {
}

CETriangleMeshShape::CETriangleMeshShape(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                         Actor* OwningActor, const DirectX::XMFLOAT3& Offset) : CEColliderShape(
	CECollisionType::TriangleMesh), Component(Component) {
}

CETriangleMeshShape::~CETriangleMeshShape() {
	CEColliderShape::~CEColliderShape();
}

CETriangleMeshShape* CETriangleMeshShape::Instance(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                                   Actor* OwningActor, const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CETriangleMeshShape(Component, PActor, OwningActor, Offset);
}

const DirectX::XMFLOAT3& CETriangleMeshShape::GetOffset() const {
}

void CETriangleMeshShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

bool CETriangleMeshShape::IsTrigger() const {
}

void CETriangleMeshShape::SetTrigger(bool IsTrigger) {
}

void CETriangleMeshShape::SetFilter(const physx::PxFilterData& filterData) {
}

void CETriangleMeshShape::DetachFromActor(physx::PxRigidActor* Actor) {
}
