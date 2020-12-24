#pragma once
#include "CEManager.h"

class CEDirect3D11Manager : public CEManager {
protected:
	CEDevice GetDevice() override;
};
