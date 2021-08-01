#pragma once

#include "CECore.h"

class CECoreRuntime : public CECore {
public:
	explicit CECoreRuntime(CEEngineConfig& EConfig);
	CECoreRuntime(CEEngineConfig& EConfig, CEPlayground* InPlayground);

	bool Create() override;
	void Run() override;
	bool Release() override;
};
