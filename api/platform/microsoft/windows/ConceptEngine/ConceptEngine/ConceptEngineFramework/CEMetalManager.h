#pragma once
#include "CEGraphicsManager.h"

class CEMetalManager : public CEGraphicsManager {
protected:
	CEDevice GetDevice() override;
};