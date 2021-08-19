#pragma once


enum class CECollisionType {
};

class CEColliderShape : public CoreObject {
	CORE_OBJECT(CEColliderShape, CoreObject);

};

class CEBoxColliderShape : public CEColliderShape {
	CORE_OBJECT(CEBoxColliderShape, CEColliderShape);

};

class CESphereColliderShape : public CEColliderShape {
	CORE_OBJECT(CESphereColliderShape, CEColliderShape);

};

class CECapsuleColliderShape : public CEColliderShape {
	CORE_OBJECT(CECapsuleColliderShape, CEColliderShape);

};

class CEConvexMeshShape : public CEColliderShape {
	CORE_OBJECT(CEConvexMeshShape, CEColliderShape);

};

class CETriangleMeshShape : public CEColliderShape {
	CORE_OBJECT(CETriangleMeshShape, CEColliderShape);

};
