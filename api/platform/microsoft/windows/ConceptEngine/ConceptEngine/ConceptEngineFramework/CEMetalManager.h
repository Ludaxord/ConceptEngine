#pragma once
#include "CEManager.h"

class CEMetalManager : public CEManager {
protected:
	CEDevice GetDevice() override;
};