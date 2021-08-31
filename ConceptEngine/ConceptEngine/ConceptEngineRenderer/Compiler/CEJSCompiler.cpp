#include "CEJSCompiler.h"

bool CEJSCompiler::Create(const std::string& SourcePath) {
	return false;
}

void CEJSCompiler::Release() {
}

void CEJSCompiler::OnSceneDestruct(CEUUID SceneID) {
}

bool CEJSCompiler::LoadRuntimeScript(const std::string& ScriptPath) {
	return false;
}

bool CEJSCompiler::LoadAppScript(const std::string& ScriptPath) {
	return false;
}

bool CEJSCompiler::ReloadScript(const std::string& ScriptPath) {
	return false;
}

void CEJSCompiler::SetSceneContext(const CEScene* scene) {
	CECompiler::SetSceneContext(scene);
}

const CEScene* CEJSCompiler::GetCurrentSceneContext() {
	return CECompiler::GetCurrentSceneContext();
}

void CEJSCompiler::OnCreateActor(Actor* in_actor) {
	CECompiler::OnCreateActor(in_actor);
}

void CEJSCompiler::OnUpdateActor(Actor* in_actor, CETimestamp timestamp) {
	CECompiler::OnUpdateActor(in_actor, timestamp);
}

void CEJSCompiler::OnPhysicsUpdate(Actor* in_actor) {
	CECompiler::OnPhysicsUpdate(in_actor);
}

void CEJSCompiler::OnCollisionBegin(Actor* in_actor) {
	CECompiler::OnCollisionBegin(in_actor);
}

void CEJSCompiler::OnCollisionEnd(Actor* in_actor) {
	CECompiler::OnCollisionEnd(in_actor);
}

void CEJSCompiler::OnTriggerBegin(Actor* in_actor) {
	CECompiler::OnTriggerBegin(in_actor);
}

void CEJSCompiler::OnTriggerEnd(Actor* in_actor) {
	CECompiler::OnTriggerEnd(in_actor);
}

bool CEJSCompiler::IsActorModuleValid(Actor* actor) {
	return CECompiler::IsActorModuleValid(actor);
}

void CEJSCompiler::OnScriptComponentDestroyed(CEUUID scene_id, CEUUID actor_id) {
	CECompiler::OnScriptComponentDestroyed(scene_id, actor_id);
}

bool CEJSCompiler::ModuleExists(const std::string& module_name) {
	return CECompiler::ModuleExists(module_name);
}

void CEJSCompiler::CreateActorScript(Actor* in_actor) {
	CECompiler::CreateActorScript(in_actor);
}

void CEJSCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

void CEJSCompiler::InstanceActorClass(Actor* in_actor) {
	CECompiler::InstanceActorClass(in_actor);
}

void CEJSCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}
