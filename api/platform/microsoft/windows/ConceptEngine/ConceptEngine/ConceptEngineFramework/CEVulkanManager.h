#pragma once
#include "CEGraphicsManager.h"

class CEVulkanManager : public CEGraphicsManager {
protected:
	CEDevice GetDevice() override;
};
