#pragma once
#include "CEManager.h"

class CEDirect3D12Manager : public CEManager {
protected:
	CEDevice GetDevice() override;
};
