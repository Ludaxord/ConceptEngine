#include "CEGraphsCompiler.h"

bool CEGraphsCompiler::Create(const std::string& SourcePath) {
	return false;
}

void CEGraphsCompiler::Release() {
}

void CEGraphsCompiler::OnSceneDestruct(CEUUID SceneID) {
}

bool CEGraphsCompiler::LoadRuntimeScript(const std::string& ScriptPath) {
	return false;
}

bool CEGraphsCompiler::LoadAppScript(const std::string& ScriptPath) {
	return false;
}

bool CEGraphsCompiler::ReloadScript(const std::string& ScriptPath) {
	return false;
}

void CEGraphsCompiler::SetSceneContext(const CEScene* scene) {
	CECompiler::SetSceneContext(scene);
}

const CEScene* CEGraphsCompiler::GetCurrentSceneContext() {
	return CECompiler::GetCurrentSceneContext();
}

void CEGraphsCompiler::OnCreateActor(Actor* in_actor) {
	CECompiler::OnCreateActor(in_actor);
}

void CEGraphsCompiler::OnUpdateActor(Actor* in_actor, CETimestamp timestamp) {
	CECompiler::OnUpdateActor(in_actor, timestamp);
}

void CEGraphsCompiler::OnPhysicsUpdate(Actor* in_actor) {
	CECompiler::OnPhysicsUpdate(in_actor);
}

void CEGraphsCompiler::OnCollisionBegin(Actor* in_actor) {
	CECompiler::OnCollisionBegin(in_actor);
}

void CEGraphsCompiler::OnCollisionEnd(Actor* in_actor) {
	CECompiler::OnCollisionEnd(in_actor);
}

void CEGraphsCompiler::OnTriggerBegin(Actor* in_actor) {
	CECompiler::OnTriggerBegin(in_actor);
}

void CEGraphsCompiler::OnTriggerEnd(Actor* in_actor) {
	CECompiler::OnTriggerEnd(in_actor);
}

bool CEGraphsCompiler::IsActorModuleValid(Actor* actor) {
	return CECompiler::IsActorModuleValid(actor);
}

void CEGraphsCompiler::OnScriptComponentDestroyed(CEUUID scene_id, CEUUID actor_id) {
	CECompiler::OnScriptComponentDestroyed(scene_id, actor_id);
}

bool CEGraphsCompiler::ModuleExists(const std::string& module_name) {
	return CECompiler::ModuleExists(module_name);
}

void CEGraphsCompiler::CreateActorScript(Actor* in_actor) {
	CECompiler::CreateActorScript(in_actor);
}

void CEGraphsCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

void CEGraphsCompiler::InstanceActorClass(Actor* in_actor) {
	CECompiler::InstanceActorClass(in_actor);
}

void CEGraphsCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}
