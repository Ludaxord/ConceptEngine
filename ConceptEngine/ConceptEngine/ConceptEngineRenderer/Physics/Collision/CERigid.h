#pragma once
#include "../Physics/PhysX/CEPhysX.h"
#include "../Physics/Transform/CERigidTransform.h"
#include "../Rendering/Resources/Mesh.h"
#include "../Scene/Components/CEPhysicsComponent.h"

//TODO: Think about making Rigid an Actor...
class CERigid {
public:
	explicit CERigid(std::string InParentName, const CERigidTransform& InParentTransform,
	                 const CERigidTransform& InLocalTransform) : ParentName(InParentName),
	                                                             ParentTransform(InParentTransform),
	                                                             LocalTransform(InLocalTransform),
	                                                             Mesh(nullptr) {
		RigidBodyName = ParentName + "_RigidBody";
	};
	virtual ~CERigid() {
		
	};

	virtual void Create() = 0;
	virtual void Create(Actor* InOwningActor) = 0;

	virtual void AddRigidStatic() {

	};

	virtual void AddRigidDynamic() {

	};

	virtual void Release() {
		
	};

public:
	std::string Name;
	std::string ParentName;

	CERigidTransform ParentTransform;
	CERigidTransform LocalTransform;

	DirectX::XMFLOAT4 Scale;
	DirectX::XMFLOAT3 PxMaterial;
	PxGeometryEnum PxGeometry;

	TSharedPtr<Mesh> Mesh;

	CEPhysicsComponent* PhysicsComponent;

protected:
	CERigidTransform WorldTransform;

	std::string RigidBodyName;
};
