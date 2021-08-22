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
	return Component.Offset;
}

//TODO: Implement..
void CEBoxColliderShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

bool CEBoxColliderShape::IsTrigger() const {
	return Component.IsTrigger;
}

//TODO: Implement..
void CEBoxColliderShape::SetTrigger(bool IsTrigger) {
}

//TODO: Implement..
void CEBoxColliderShape::SetFilter(const physx::PxFilterData& filterData) {
}

//TODO: Implement..
void CEBoxColliderShape::DetachFromActor(physx::PxRigidActor* Actor) {
}

//TODO: Implement..
CESphereColliderShape::CESphereColliderShape(CEColliderSphereComponent& Component,
                                             const CEPhysicsActor& PActor,
                                             Actor* OwningActor,
                                             const DirectX::XMFLOAT3& Offset) :
	CEColliderShape(CECollisionType::Sphere),
	Component(Component) {
}

//TODO: Implement..
CESphereColliderShape::~CESphereColliderShape() {
	CEColliderShape::~CEColliderShape();
}

//TODO: Implement..
CESphereColliderShape* CESphereColliderShape::Instance(CEColliderSphereComponent& Component,
                                                       const CEPhysicsActor& PActor,
                                                       Actor* OwningActor,
                                                       const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CESphereColliderShape(Component, PActor, OwningActor, Offset);
}

//TODO: Implement..
const DirectX::XMFLOAT3& CESphereColliderShape::GetOffset() const {
}

//TODO: Implement..
void CESphereColliderShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

//TODO: Implement..
bool CESphereColliderShape::IsTrigger() const {
}

//TODO: Implement..
void CESphereColliderShape::SetTrigger(bool IsTrigger) {
}

//TODO: Implement..
void CESphereColliderShape::SetFilter(const physx::PxFilterData& filterData) {
}

//TODO: Implement..
void CESphereColliderShape::DetachFromActor(physx::PxRigidActor* Actor) {
}

//TODO: Implement..
CECapsuleColliderShape::CECapsuleColliderShape(CEColliderCapsuleComponent& Component,
                                               const CEPhysicsActor& PActor,
                                               Actor* OwningActor,
                                               const DirectX::XMFLOAT3& Offset) : CEColliderShape(
		CECollisionType::Capsule),
	Component(Component) {
}

//TODO: Implement..
CECapsuleColliderShape::~CECapsuleColliderShape() {
	CEColliderShape::~CEColliderShape();
}

//TODO: Implement..
CECapsuleColliderShape* CECapsuleColliderShape::Instance(CEColliderCapsuleComponent& Component,
                                                         const CEPhysicsActor& PActor, Actor* OwningActor,
                                                         const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CECapsuleColliderShape(Component, PActor, OwningActor, Offset);
}

//TODO: Implement..
const DirectX::XMFLOAT3& CECapsuleColliderShape::GetOffset() const {
}

//TODO: Implement..
void CECapsuleColliderShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

//TODO: Implement..
bool CECapsuleColliderShape::IsTrigger() const {
}

//TODO: Implement..
void CECapsuleColliderShape::SetTrigger(bool IsTrigger) {
}

//TODO: Implement..
void CECapsuleColliderShape::SetFilter(const physx::PxFilterData& filterData) {
}

//TODO: Implement..
void CECapsuleColliderShape::DetachFromActor(physx::PxRigidActor* Actor) {
}

//TODO: Implement..
CEConvexMeshShape::CEConvexMeshShape(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                     Actor* OwningActor, const DirectX::XMFLOAT3& Offset) :
	CEColliderShape(CECollisionType::ConvexMesh), Component(Component) {
}

//TODO: Implement..
CEConvexMeshShape::~CEConvexMeshShape() {
	CEColliderShape::~CEColliderShape();
}

//TODO: Implement..
CEConvexMeshShape* CEConvexMeshShape::Instance(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                               Actor* OwningActor, const DirectX::XMFLOAT3& Offset) {
	return DBG_NEW CEConvexMeshShape(Component, PActor, OwningActor, Offset);
}

//TODO: Implement..
const DirectX::XMFLOAT3& CEConvexMeshShape::GetOffset() const {
}

//TODO: Implement..
void CEConvexMeshShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

//TODO: Implement..
bool CEConvexMeshShape::IsTrigger() const {
}

//TODO: Implement..
void CEConvexMeshShape::SetTrigger(bool IsTrigger) {
}

//TODO: Implement..
void CEConvexMeshShape::SetFilter(const physx::PxFilterData& filterData) {
}

//TODO: Implement..
void CEConvexMeshShape::DetachFromActor(physx::PxRigidActor* Actor) {
}

//TODO: Implement..
CETriangleMeshShape::CETriangleMeshShape(CEColliderMeshComponent& Component, const CEPhysicsActor& PActor,
                                         Actor* OwningActor, const DirectX::XMFLOAT3& Offset) : CEColliderShape(
	CECollisionType::TriangleMesh), Component(Component) {
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

//TODO: Implement..
const DirectX::XMFLOAT3& CETriangleMeshShape::GetOffset() const {
}

//TODO: Implement..
void CETriangleMeshShape::SetOffset(const DirectX::XMFLOAT3& Offset) {
}

//TODO: Implement..
bool CETriangleMeshShape::IsTrigger() const {
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
