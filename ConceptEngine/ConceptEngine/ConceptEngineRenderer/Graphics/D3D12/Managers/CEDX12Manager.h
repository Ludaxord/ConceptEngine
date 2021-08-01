#pragma once
#include "Graphics/Generic/Managers/CEGraphicsManager.h"

class CEDX12Manager : public CEGraphicsManager {
public:
	CEDX12Manager();
	bool Create() override;
	void Release() override;
};
