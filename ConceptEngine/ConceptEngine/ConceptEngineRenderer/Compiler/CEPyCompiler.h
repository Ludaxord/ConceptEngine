#pragma once
#include "CECompiler.h"

class CEPyCompiler : public CECompiler {
public:
	void Create(const std::string& SourcePath) override;
	void Release() override;
	void OnSceneDestruct(CEUUID SceneID) override;
	bool LoadRuntimeScript(const std::string& ScriptPath) override;
	bool LoadAppScript(const std::string& ScriptPath) override;
	bool ReloadScript(const std::string& ScriptPath) override;
	void SetSceneContext(const CEScene* Scene) override;
	const CEScene* GetCurrentSceneContext() override;
	void OnCreateActor(Actor* InActor) override;
	void OnUpdateActor(Actor* InActor, CETimestamp Timestamp) override;
	void OnPhysicsUpdate(Actor* InActor) override;
	void OnCollisionBegin(Actor* InActor) override;
	void OnCollisionEnd(Actor* InActor) override;
	void OnTriggerBegin(Actor* InActor) override;
	void OnTriggerEnd(Actor* InActor) override;
	bool IsActorModuleValid(Actor* Actor) override;
	void OnScriptComponentDestroyed(CEUUID SceneID, CEUUID ActorID) override;
	bool ModuleExists(const std::string& ModuleName) override;
	void CreateActorScript(Actor* InActor) override;
	void ReleaseActorScript(Actor* InActor, const std::string& ModuleName) override;
	void InstanceActorClass(Actor* InActor) override;
	void OnEditorRender() override;
};
