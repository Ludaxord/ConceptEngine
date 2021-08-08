#pragma once
#include "../Config/CEGlobalConfigs.h"
#include "../Time/CETimestamp.h"
#include "../Managers/CEManager.h"

class CEPhysics;
class CEPlatform;
class CEGraphics;
class CEPlayground;
class CECompiler;

class CECore {
public:
	CECore(CEEngineConfig& EConfig);
	CECore(CEEngineConfig& EConfig, CEPlayground* InPlayground);
	virtual ~CECore() = default;

	virtual bool Create();

	virtual void Run() = 0;
	virtual void Update(CETimestamp DeltaTime) = 0;
	virtual bool Release() = 0;

	static CEGraphics* GetGraphics();
	static CECompiler* GetCompiler();
	static CEPlatform* GetPlatform();
	static CEPhysics* GetPhysics();
	static CEPlayground* GetPlayground();
	
	inline static bool bIsRunning = false;
	inline static bool bEnableDebug = false;

protected:
	inline static CEGraphics* GGraphics;
	inline static CECompiler* GCompiler;
	inline static CEPlatform* GPlatform;
	inline static CEPhysics* GPhysics;

private:
	CEGraphics* SetGraphicsAPI(GraphicsAPI GApi);
	CECompiler* SetLanguageCompiler(ScriptingLanguage SLanguage);
	CEPlatform* SetPlatform(PlatformBoot PBoot);
	CEPhysics* SetPhysics(PhysicsLibrary PLibrary);


};
