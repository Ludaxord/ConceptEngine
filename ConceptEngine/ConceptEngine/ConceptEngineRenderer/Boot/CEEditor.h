#pragma once

#include "CECore.h"

class CEEditor : public CECore {
public:
	explicit CEEditor(CEEngineConfig& EConfig);
	CEEditor(CEEngineConfig& EConfig, CEPlayground* InPlayground);

	bool Create() override;
	void Run() override;
	void Update(CETimestamp DeltaTime) override;
	bool Release() override;
};
