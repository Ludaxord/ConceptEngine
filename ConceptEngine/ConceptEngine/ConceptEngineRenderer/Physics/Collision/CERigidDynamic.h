#pragma once
#include "CERigid.h"

class CERigidDynamic : public CERigid {
public:
	CERigidDynamic(
		std::string& InParentName,
		const CERigidTransform& InParentTransform,
		const CERigidTransform& InLocalTransform
	);
	~CERigidDynamic();
	void CreatePhysicsMesh();
	void AddRigidDynamic() override;
	void Release() override;
	void Create() override;
	void CreatePhysicsComponent(Actor* InOwningActor);
	void Create(Actor* InOwningActor) override;
	void Update(CERigidTransform& ParentTransform);

	void AddForce(DirectX::XMFLOAT3 Force);
	void SetRigidDynamicLockFlag(int Axis, bool DynamicLockFlag);
	void SetAngularDamping(float AngularDamping);
	void SetLinearVelocity(DirectX::XMFLOAT3 Velocity);

private:
	void PushTransform(const CERigidTransform& ParentTransform);
	void PullTransform();

public:
	float Density;

	bool IsKinematic = false;

private:
	DirectX::XMFLOAT4X4 LocalMatrixReverse;
};
