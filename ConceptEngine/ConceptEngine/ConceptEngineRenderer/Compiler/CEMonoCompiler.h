#pragma once
#include "CECompiler.h"
#include "CEMonoScriptModuleField.h"

struct CEMonoScriptClass;

extern "C" {
typedef struct _MonoObject MonoObject;
typedef struct _MonoClassField MonoClassField;
typedef struct _MonoClass MonoClass;
}

struct CEActorInstance {
	CEMonoScriptClass* ScriptClass = nullptr;

	uint32 Handle = 0;
	CEScene* SceneInstance = nullptr;

	MonoObject* GetInstance();

	bool IsRuntimeAvailable() const;
};

struct CEInstanceData {
	CEActorInstance Instance;
	CEMonoScriptModuleFieldMap ModuleFieldMap;
};

using CEActorInstanceMap = std::unordered_map<CEUUID, std::unordered_map<CEUUID, CEInstanceData>>;

class CEMonoCompiler : public CECompiler {
public:
	bool Create(const std::string& SourcePath) override;
	void Release() override;
	void OnSceneDestruct(CEUUID SceneID) override;
	bool LoadRuntimeScript(const std::string& ScriptPath) override;
	bool LoadAppScript(const std::string& ScriptPath) override;
	bool ReloadScript(const std::string& ScriptPath) override;
	void SetSceneContext(CEScene* Scene) override;
	const CEScene* GetCurrentSceneContext() override;
	void OnCreateActor(Actor* InActor) override;
	void OnUpdateActor(Actor* InActor, CETimestamp Timestamp) override;
	void OnPhysicsUpdate(Actor* InActor, float FixedTimestamp) override;
	void OnCollisionBegin(Actor* InActor) override;
	void OnCollisionEnd(Actor* InActor) override;
	void OnTriggerBegin(Actor* InActor) override;
	void OnTriggerEnd(Actor* InActor) override;
	bool IsActorModuleValid(Actor* Actor) override;
	void OnScriptComponentDestroyed(CEUUID SceneID, CEUUID ActorID) override;
	bool ModuleExists(const std::string& ModuleName) override;
	void CreateActorScript(Actor* InActor) override;
	void ReleaseActorScript(Actor* InActor, const std::string& ModuleName) override;
	void InstantiateActorClass(Actor* InActor) override;
	void OnEditorRender() override;

	CEActorInstanceMap GetActorInstanceMap();
	CEInstanceData& GetInstanceData(CEUUID SceneID, CEUUID ActorID);

	MonoObject* Construct(std::string& FullName, bool CalConstructor = true, void** Params = nullptr);
	MonoClass* GetCoreClass(std::string& FullName);
};
