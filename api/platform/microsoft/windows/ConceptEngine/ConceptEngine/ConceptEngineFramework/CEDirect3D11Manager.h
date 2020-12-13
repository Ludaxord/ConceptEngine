#pragma once
#include "CEGraphicsManager.h"

class CEDirect3D11Manager : public CEGraphicsManager {
protected:
	CEDevice GetDevice() override;
};
