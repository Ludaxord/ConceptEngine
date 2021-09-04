#include "CEMonoCompiler.h"

#include <mono/metadata/object-forward.h>
#include <mono/utils/mono-forward.h>

static MonoDomain* CurrentMonoDomain = nullptr;
static MonoDomain* NewMonoDomain = nullptr;
static std::string CoreAssemblyPath;
static CEScene* SceneContext;

MonoImage* AppAssemblyImage = nullptr;
MonoImage* CoreAssemblyImage = nullptr;

static MonoMethod* GetMethod(MonoImage* Image, const std::string& MethodDesc);

static MonoMethod* ExceptionMethod = nullptr;
static MonoClass* EntityClass = nullptr;

struct CEMonoActorClass {
	std::string FullName;
	std::string ClassName;
	std::string NamespaceName;

	MonoClass* Class = nullptr;
	MonoMethod* Constructor = nullptr;
	MonoMethod* OnCreate = nullptr;
	MonoMethod* OnRelease = nullptr;
	MonoMethod* OnUpdate = nullptr;
	MonoMethod* OnPhysicsUpdate = nullptr;

	MonoMethod* OnCollisionBegin = nullptr;
	MonoMethod* OnCollisionEnd = nullptr;
	MonoMethod* OnTriggerBegin = nullptr;
	MonoMethod* OnTriggerEnd = nullptr;
	MonoMethod* OnCollision2DBegin = nullptr;
	MonoMethod* OnCollision2DEnd = nullptr;

	void CreateClassMethods(MonoImage* Image) {
		Constructor = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:.ctr(ulong)");
		OnCreate = GetMethod(Image, ":OnCreate()");
		OnRelease = GetMethod(Image, ":OnRelease()");
		OnUpdate = GetMethod(Image, ":OnUpdate(Single)");
		OnPhysicsUpdate = GetMethod(Image, ":OnPhysicsUpdate(Single)");

		OnCollisionBegin = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnCollisionBegin(Single)");
		OnCollisionEnd = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnCollisionEnd(Single)");
		OnTriggerBegin = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnTriggerBegin(Single)");
		OnTriggerEnd = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnTriggerEnd(Single)");
		OnCollision2DBegin = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnCollision2DBegin(Single)");
		OnCollision2DEnd = GetMethod(CoreAssemblyImage, "ConceptEngine.CEActor:OnCollision2DEnd(Single)");
	}
};

//TODO: Implement...
bool CEMonoCompiler::Create(const std::string& SourcePath) {
	return false;
}

//TODO: Implement...
void CEMonoCompiler::Release() {
}

//TODO: Implement...
void CEMonoCompiler::OnSceneDestruct(CEUUID SceneID) {
}

//TODO: Implement...
bool CEMonoCompiler::LoadRuntimeScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
bool CEMonoCompiler::LoadAppScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
bool CEMonoCompiler::ReloadScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
void CEMonoCompiler::SetSceneContext(const CEScene* scene) {
	CECompiler::SetSceneContext(scene);
}

//TODO: Implement...
const CEScene* CEMonoCompiler::GetCurrentSceneContext() {
	return CECompiler::GetCurrentSceneContext();
}

//TODO: Implement...
void CEMonoCompiler::OnCreateActor(Actor* in_actor) {
	CECompiler::OnCreateActor(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnUpdateActor(Actor* in_actor, CETimestamp timestamp) {
	CECompiler::OnUpdateActor(in_actor, timestamp);
}

//TODO: Implement...
void CEMonoCompiler::OnPhysicsUpdate(Actor* in_actor) {
	CECompiler::OnPhysicsUpdate(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnCollisionBegin(Actor* in_actor) {
	CECompiler::OnCollisionBegin(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnCollisionEnd(Actor* in_actor) {
	CECompiler::OnCollisionEnd(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnTriggerBegin(Actor* in_actor) {
	CECompiler::OnTriggerBegin(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnTriggerEnd(Actor* in_actor) {
	CECompiler::OnTriggerEnd(in_actor);
}

//TODO: Implement...
bool CEMonoCompiler::IsActorModuleValid(Actor* actor) {
	return CECompiler::IsActorModuleValid(actor);
}

//TODO: Implement...
void CEMonoCompiler::OnScriptComponentDestroyed(CEUUID scene_id, CEUUID actor_id) {
	CECompiler::OnScriptComponentDestroyed(scene_id, actor_id);
}

//TODO: Implement...
bool CEMonoCompiler::ModuleExists(const std::string& module_name) {
	return CECompiler::ModuleExists(module_name);
}

//TODO: Implement...
void CEMonoCompiler::CreateActorScript(Actor* in_actor) {
	CECompiler::CreateActorScript(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

//TODO: Implement...
void CEMonoCompiler::InstanceActorClass(Actor* in_actor) {
	CECompiler::InstanceActorClass(in_actor);
}

//TODO: Implement...
void CEMonoCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}
