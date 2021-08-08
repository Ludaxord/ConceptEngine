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
	void AddRigidDynamic() override;
	void Release() override;
};
