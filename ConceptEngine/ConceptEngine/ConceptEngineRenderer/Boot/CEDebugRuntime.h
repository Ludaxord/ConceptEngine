#pragma once
#include "CECoreRuntime.h"

class CEDebugRuntime : public CECoreRuntime {
public:
	explicit CEDebugRuntime(CEEngineConfig& EConfig);
	CEDebugRuntime(CEEngineConfig& EConfig, CEPlayground* InPlayground);

	bool Create() override;
	void Update(CETimestamp DeltaTime) override;
};
