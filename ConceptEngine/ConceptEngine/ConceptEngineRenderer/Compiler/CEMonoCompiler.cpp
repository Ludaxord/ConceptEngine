#include "CEMonoCompiler.h"

bool CEMonoCompiler::Create(const std::string& SourcePath) {
	return false;
}

void CEMonoCompiler::Release() {
}

void CEMonoCompiler::OnSceneDestruct(CEUUID SceneID) {
}

bool CEMonoCompiler::LoadRuntimeScript(const std::string& ScriptPath) {
	return false;
}

bool CEMonoCompiler::LoadAppScript(const std::string& ScriptPath) {
	return false;
}

bool CEMonoCompiler::ReloadScript(const std::string& ScriptPath) {
	return false;
}

void CEMonoCompiler::SetSceneContext(const CEScene* scene) {
	CECompiler::SetSceneContext(scene);
}

const CEScene* CEMonoCompiler::GetCurrentSceneContext() {
	return CECompiler::GetCurrentSceneContext();
}

void CEMonoCompiler::OnCreateActor(Actor* in_actor) {
	CECompiler::OnCreateActor(in_actor);
}

void CEMonoCompiler::OnUpdateActor(Actor* in_actor, CETimestamp timestamp) {
	CECompiler::OnUpdateActor(in_actor, timestamp);
}

void CEMonoCompiler::OnPhysicsUpdate(Actor* in_actor) {
	CECompiler::OnPhysicsUpdate(in_actor);
}

void CEMonoCompiler::OnCollisionBegin(Actor* in_actor) {
	CECompiler::OnCollisionBegin(in_actor);
}

void CEMonoCompiler::OnCollisionEnd(Actor* in_actor) {
	CECompiler::OnCollisionEnd(in_actor);
}

void CEMonoCompiler::OnTriggerBegin(Actor* in_actor) {
	CECompiler::OnTriggerBegin(in_actor);
}

void CEMonoCompiler::OnTriggerEnd(Actor* in_actor) {
	CECompiler::OnTriggerEnd(in_actor);
}

bool CEMonoCompiler::IsActorModuleValid(Actor* actor) {
	return CECompiler::IsActorModuleValid(actor);
}

void CEMonoCompiler::OnScriptComponentDestroyed(CEUUID scene_id, CEUUID actor_id) {
	CECompiler::OnScriptComponentDestroyed(scene_id, actor_id);
}

bool CEMonoCompiler::ModuleExists(const std::string& module_name) {
	return CECompiler::ModuleExists(module_name);
}

void CEMonoCompiler::CreateActorScript(Actor* in_actor) {
	CECompiler::CreateActorScript(in_actor);
}

void CEMonoCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

void CEMonoCompiler::InstanceActorClass(Actor* in_actor) {
	CECompiler::InstanceActorClass(in_actor);
}

void CEMonoCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}
