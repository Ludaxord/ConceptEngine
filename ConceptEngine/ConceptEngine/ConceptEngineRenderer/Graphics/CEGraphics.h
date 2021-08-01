#pragma once
#include <boost/function.hpp>

#include "Generic/Managers/CEGraphicsManager.h"
#include "Generic/Managers/CEMeshManager.h"
#include "Generic/Managers/CETextureManager.h"
#include "Managers/CEManager.h"
#include "Rendering/DebugUI.h"
#include "Time/CETimestamp.h"

class CEGraphics {
public:
	CEGraphics();
	virtual ~CEGraphics() = default;

	virtual bool Create();
	virtual bool CreateManagers();
	virtual bool Release();

	virtual void Update(CETimestamp DeltaTime, boost::function<void()> ExecuteFunction) = 0;
	virtual bool Resize() = 0;

	static CEDebugUI* GetDebugUI();

	CEManager* GetManager(ManagerType Type) const;

protected:
	CEGraphicsManager* GraphicsManager;
	CETextureManager* TextureManager;
	CEMeshManager* MeshManager;

	inline static CEDebugUI* DebugUI;

private:
	virtual bool CreateGraphicsManager() = 0;
	virtual bool CreateTextureManager() = 0;
	virtual bool CreateMeshManager() = 0;
	virtual bool CreateShaderCompiler() = 0;
	virtual bool CreateDebugUI() = 0;
};
