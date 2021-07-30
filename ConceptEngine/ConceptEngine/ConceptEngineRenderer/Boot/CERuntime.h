#pragma once
#include "CECoreRuntime.h"


class CERuntime : public CECoreRuntime {
public:
	explicit CERuntime(CEEngineConfig& EConfig);
	CERuntime(CEEngineConfig& EConfig, CEPlayground* InPlayground);

	void Run() override;
	void Update(CETimestamp DeltaTime) override;
	bool Release() override;
};
