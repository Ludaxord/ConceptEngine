#pragma once
#include "CEGraphicsManager.h"

class CEDirect3D12Manager : public CEGraphicsManager {
protected:
	CEDevice GetDevice() override;
};
