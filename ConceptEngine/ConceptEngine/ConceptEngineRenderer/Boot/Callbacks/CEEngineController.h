#pragma once
#include "CEPlatformCallbacks.h"


class CEEngineController final : public CEPlatformCallbacks {
public:
	bool Create();
	bool Release();

	void Exit();
};

extern CEEngineController GEngineController;