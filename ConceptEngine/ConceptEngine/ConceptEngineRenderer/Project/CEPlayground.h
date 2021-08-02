#pragma once
#include "../Time/CETimestamp.h"

class CEPlayground {
public:
	CEPlayground();
	virtual ~CEPlayground() = default;

	virtual bool Create();
	virtual void Update(CETimestamp DeltaTime);
	virtual bool Release();

	class CEScene* Scene = nullptr;
};


extern CEPlayground* CreatePlayground();

extern CEPlayground* GPlayground;
