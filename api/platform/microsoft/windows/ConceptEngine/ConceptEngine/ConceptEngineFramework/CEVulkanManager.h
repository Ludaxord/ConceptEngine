#pragma once
#include "CEManager.h"

class CEVulkanManager : public CEManager {
protected:
	CEDevice GetDevice() override;
};
