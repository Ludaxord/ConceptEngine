#pragma once
#include <PxFiltering.h>
#include <PxMaterial.h>

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

	virtual void SetFilter(const physx::PxFilterData& filterData) = 0;


protected:
	CECollisionType Type;
	physx::PxMaterial* Material;

};

class CEBoxColliderShape : public CEColliderShape {
	CORE_OBJECT(CEBoxColliderShape, CEColliderShape);
public:
	static CEBoxColliderShape* Instance();
};

class CESphereColliderShape : public CEColliderShape {
	CORE_OBJECT(CESphereColliderShape, CEColliderShape);
public:
	static CESphereColliderShape* Instance();

};

class CECapsuleColliderShape : public CEColliderShape {
	CORE_OBJECT(CECapsuleColliderShape, CEColliderShape);
public:
	static CECapsuleColliderShape* Instance();

};

class CEConvexMeshShape : public CEColliderShape {
	CORE_OBJECT(CEConvexMeshShape, CEColliderShape);
public:
	static CEConvexMeshShape* Instance();

};

class CETriangleMeshShape : public CEColliderShape {
	CORE_OBJECT(CETriangleMeshShape, CEColliderShape);
public:
	static CETriangleMeshShape* Instance();

};
