#pragma once
#include "Graphics/CEGraphics.h"

class CEDirectX12 : public CEGraphics {
public:
	CEDirectX12();
	~CEDirectX12() override;
	void Update(CETimestamp DeltaTime, boost::function<void()> ExecuteFunction) override;
	bool Resize() override;
private:
	bool CreateGraphicsManager() override;
	bool CreateTextureManager() override;
	bool CreateMeshManager() override;
	bool CreateShaderCompiler() override;
	bool CreateDebugUI() override;
public:
	bool Create() override;
	bool CreateManagers() override;
	bool Release() override;
};
