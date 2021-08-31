#include "CEPyCompiler.h"

bool CEPyCompiler::Create(const std::string& SourcePath) {
	return false;
}

void CEPyCompiler::Release() {
}

void CEPyCompiler::OnSceneDestruct(CEUUID SceneID) {
}

bool CEPyCompiler::LoadRuntimeScript(const std::string& ScriptPath) {
	return false;
}

bool CEPyCompiler::LoadAppScript(const std::string& ScriptPath) {
	return false;
}

bool CEPyCompiler::ReloadScript(const std::string& ScriptPath) {
	return false;
}

void CEPyCompiler::SetSceneContext(const CEScene* scene) {
	CECompiler::SetSceneContext(scene);
}

const CEScene* CEPyCompiler::GetCurrentSceneContext() {
	return CECompiler::GetCurrentSceneContext();
}

void CEPyCompiler::OnCreateActor(Actor* in_actor) {
	CECompiler::OnCreateActor(in_actor);
}

void CEPyCompiler::OnUpdateActor(Actor* in_actor, CETimestamp timestamp) {
	CECompiler::OnUpdateActor(in_actor, timestamp);
}

void CEPyCompiler::OnPhysicsUpdate(Actor* in_actor) {
	CECompiler::OnPhysicsUpdate(in_actor);
}

void CEPyCompiler::OnCollisionBegin(Actor* in_actor) {
	CECompiler::OnCollisionBegin(in_actor);
}

void CEPyCompiler::OnCollisionEnd(Actor* in_actor) {
	CECompiler::OnCollisionEnd(in_actor);
}

void CEPyCompiler::OnTriggerBegin(Actor* in_actor) {
	CECompiler::OnTriggerBegin(in_actor);
}

void CEPyCompiler::OnTriggerEnd(Actor* in_actor) {
	CECompiler::OnTriggerEnd(in_actor);
}

bool CEPyCompiler::IsActorModuleValid(Actor* actor) {
	return CECompiler::IsActorModuleValid(actor);
}

void CEPyCompiler::OnScriptComponentDestroyed(CEUUID scene_id, CEUUID actor_id) {
	CECompiler::OnScriptComponentDestroyed(scene_id, actor_id);
}

bool CEPyCompiler::ModuleExists(const std::string& module_name) {
	return CECompiler::ModuleExists(module_name);
}

void CEPyCompiler::CreateActorScript(Actor* in_actor) {
	CECompiler::CreateActorScript(in_actor);
}

void CEPyCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

void CEPyCompiler::InstanceActorClass(Actor* in_actor) {
	CECompiler::InstanceActorClass(in_actor);
}

void CEPyCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}
