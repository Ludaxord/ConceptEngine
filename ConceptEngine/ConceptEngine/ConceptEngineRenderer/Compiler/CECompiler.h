#pragma once
#include "Core/Containers/CEUUID.h"
#include "Scene/CEScene.h"

struct CEActorInstanceData {

};

struct CESceneActorInstanceData {

};

class CECompiler {
public:
	virtual void Create(const std::string& SourcePath) = 0;
	virtual void Release() = 0;

	virtual void OnSceneDestruct(CEUUID SceneID) = 0;

	virtual bool LoadRuntimeScript(const std::string& ScriptPath) = 0;
	virtual bool LoadAppScript(const std::string& ScriptPath) = 0;
	virtual bool ReloadScript(const std::string& ScriptPath) = 0;

	virtual void SetSceneContext(const CEScene* Scene);
	virtual const CEScene* GetCurrentSceneContext();

	virtual void OnCreateActor(Actor* InActor);
	virtual void OnUpdateActor(Actor* InActor, CETimestamp Timestamp);
	virtual void OnPhysicsUpdate(Actor* InActor);

	virtual void OnCollisionBegin(Actor* InActor);
	virtual void OnCollisionEnd(Actor* InActor);

	virtual void OnTriggerBegin(Actor* InActor);
	virtual void OnTriggerEnd(Actor* InActor);

	virtual bool IsActorModuleValid(Actor* Actor);

	virtual void OnScriptComponentDestroyed(CEUUID SceneID, CEUUID ActorID);

	virtual bool ModuleExists(const std::string& ModuleName);
	virtual void CreateActorScript(Actor* InActor);
	virtual void ReleaseActorScript(Actor* InActor, const std::string& ModuleName);
	virtual void InstanceActorClass(Actor* InActor);

	virtual void OnEditorRender();
};
