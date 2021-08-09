#pragma once
#include "CERigid.h"

class CERigidStatic : public CERigid {
public:
	CERigidStatic();

	CERigidStatic(std::string& InParentName, const CERigidTransform& InParentTransform,
	              const CERigidTransform& InLocalTransform);
	~CERigidStatic() override;
	void AddRigidStatic() override;
	void Release() override;
	void Create() override;
	void Create(Actor* InOwningActor) override;

	void CreatePhysicsMesh();
	void CreatePhysicsComponent(Actor* InOwningActor);
};
