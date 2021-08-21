#pragma once
#include <PxFiltering.h>
#include <PxMaterial.h>

#include "Scene/Components/CEColliderBoxComponent.h"
#include "Scene/Components/CEColliderCapsuleComponent.h"
#include "Scene/Components/CEColliderMeshComponent.h"
#include "Scene/Components/CEColliderSphereComponent.h"

enum class CECollisionType {
	Box,
	Sphere,
	Capsule,
	ConvexMesh,
	TriangleMesh
};

//TODO: Implement
class CEPhysicsMaterial;

class CEColliderShape : public CoreObject {
	CORE_OBJECT(CEColliderShape, CoreObject);
protected:
	CEColliderShape(CECollisionType InType) : Type(InType), Material() {

	}

public:
	virtual ~CEColliderShape() {

	}

	void Release() {
		Material->release();
	}

	void SetMaterial(CEPhysicsMaterial* Material);

	virtual const DirectX::XMFLOAT3& GetOffset() const = 0;
	virtual void SetOffset(const DirectX::XMFLOAT3& Offset) = 0;

	virtual bool IsTrigger() const = 0;
	virtual void SetTrigger(bool IsTrigger) = 0;

	virtual void SetFilter(const physx::PxFilterData& filterData) = 0;

	virtual void DetachFromActor(physx::PxRigidActor* Actor) = 0;

	physx::PxMaterial& GetMaterial() const {
		return *Material;
	}

	bool IsValid() const {
		return Material != nullptr;
	}

protected:
	CECollisionType Type;
	physx::PxMaterial* Material;

};

class CEPhysicsActor;

class CEBoxColliderShape : public CEColliderShape {
	CORE_OBJECT(CEBoxColliderShape, CEColliderShape);
public:
	CEBoxColliderShape(
		CEColliderBoxComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	~CEBoxColliderShape();

	static CEBoxColliderShape* Instance(
		CEColliderBoxComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);

	const DirectX::XMFLOAT3& GetOffset() const override;
	void SetOffset(const DirectX::XMFLOAT3& Offset) override;

	bool IsTrigger() const override;
	void SetTrigger(bool IsTrigger) override;

	void SetFilter(const physx::PxFilterData& filterData) override;
	void DetachFromActor(physx::PxRigidActor* Actor) override;

private:
	CEColliderBoxComponent& Component;
	physx::PxShape* Shape;
};

class CESphereColliderShape : public CEColliderShape {
	CORE_OBJECT(CESphereColliderShape, CEColliderShape);
public:
	CESphereColliderShape(
		CEColliderSphereComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	~CESphereColliderShape();

	static CESphereColliderShape* Instance(
		CEColliderSphereComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);

	const DirectX::XMFLOAT3& GetOffset() const override;
	void SetOffset(const DirectX::XMFLOAT3& Offset) override;

	bool IsTrigger() const override;
	void SetTrigger(bool IsTrigger) override;

	void SetFilter(const physx::PxFilterData& filterData) override;
	void DetachFromActor(physx::PxRigidActor* Actor) override;

private:
	CEColliderSphereComponent& Component;
	physx::PxShape* Shape;

};

class CECapsuleColliderShape : public CEColliderShape {
	CORE_OBJECT(CECapsuleColliderShape, CEColliderShape);
public:
	CECapsuleColliderShape(
		CEColliderCapsuleComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	~CECapsuleColliderShape();

	static CECapsuleColliderShape* Instance(
		CEColliderCapsuleComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	const DirectX::XMFLOAT3& GetOffset() const override;
	void SetOffset(const DirectX::XMFLOAT3& Offset) override;
	bool IsTrigger() const override;
	void SetTrigger(bool IsTrigger) override;
	void SetFilter(const physx::PxFilterData& filterData) override;
	void DetachFromActor(physx::PxRigidActor* Actor) override;

private:
	CEColliderCapsuleComponent& Component;
	physx::PxShape* Shape;
};

class CEConvexMeshShape : public CEColliderShape {
	CORE_OBJECT(CEConvexMeshShape, CEColliderShape);
public:
	CEConvexMeshShape(
		CEColliderMeshComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	~CEConvexMeshShape();

	static CEConvexMeshShape* Instance(
		CEColliderMeshComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	const DirectX::XMFLOAT3& GetOffset() const override;
	void SetOffset(const DirectX::XMFLOAT3& Offset) override;
	bool IsTrigger() const override;
	void SetTrigger(bool IsTrigger) override;
	void SetFilter(const physx::PxFilterData& filterData) override;
	void DetachFromActor(physx::PxRigidActor* Actor) override;

private:
	CEColliderMeshComponent& Component;
	physx::PxShape* Shape;
};

class CETriangleMeshShape : public CEColliderShape {
	CORE_OBJECT(CETriangleMeshShape, CEColliderShape);
public:
	CETriangleMeshShape(
		CEColliderMeshComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	~CETriangleMeshShape();

	static CETriangleMeshShape* Instance(
		CEColliderMeshComponent& Component,
		const CEPhysicsActor& PActor,
		Actor* OwningActor,
		const DirectX::XMFLOAT3& Offset = XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	const DirectX::XMFLOAT3& GetOffset() const override;
	void SetOffset(const DirectX::XMFLOAT3& Offset) override;
	bool IsTrigger() const override;
	void SetTrigger(bool IsTrigger) override;
	void SetFilter(const physx::PxFilterData& filterData) override;
	void DetachFromActor(physx::PxRigidActor* Actor) override;

private:
	CEColliderMeshComponent& Component;
	physx::PxShape* Shape;
};
