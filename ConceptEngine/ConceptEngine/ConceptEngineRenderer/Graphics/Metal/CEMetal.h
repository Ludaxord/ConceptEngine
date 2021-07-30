#pragma once
#include "Graphics/CEGraphics.h"

class CEMetal : public CEGraphics {
public:
	bool Update(CETimestamp DeltaTime, boost::function<void()> ExecuteFunction) override;
	bool Resize() override;
private:
	bool CreateGraphicsManager() override;
	bool CreateTextureManager() override;
	bool CreateMeshManager() override;
	bool CreateShaderCompiler() override;
	bool CreateDebugUI() override;
};
