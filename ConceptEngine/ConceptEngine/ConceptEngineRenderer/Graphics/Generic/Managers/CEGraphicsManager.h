#pragma once
#include "Managers/CEManager.h"

class CEGraphicsManager : public CEManager {

public:
	bool Create() override;
	void Release() override;
};
