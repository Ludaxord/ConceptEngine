#pragma once
#include "CERigid.h"

class CERigidDynamic : public CERigid {
public:
	CERigidDynamic();
	~CERigidDynamic();
	void AddRigidDynamic() override;
	void Release() override;
};
