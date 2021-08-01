#pragma once
#include "CECoreRuntime.h"


class CERuntime : public CECoreRuntime {
public:
	explicit CERuntime(CEEngineConfig& EConfig);
	CERuntime(CEEngineConfig& EConfig, CEPlayground* InPlayground);

	bool Create() override;
	void Update(CETimestamp DeltaTime) override;
};
