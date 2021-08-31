#include "CECppCompiler.h"

bool CECppCompiler::Create(const std::string& SourcePath) {
	return false;
}

void CECppCompiler::Release() {
}

void CECppCompiler::OnSceneDestruct(CEUUID SceneID) {
}

bool CECppCompiler::LoadRuntimeScript(const std::string& ScriptPath) {
	return false;
}

bool CECppCompiler::LoadAppScript(const std::string& ScriptPath) {
	return false;
}

bool CECppCompiler::ReloadScript(const std::string& ScriptPath) {
	return false;
}

void CECppCompiler::SetSceneContext(const CEScene* scene) {
	CECompiler::SetSceneContext(scene);
}

const CEScene* CECppCompiler::GetCurrentSceneContext() {
	return CECompiler::GetCurrentSceneContext();
}

void CECppCompiler::OnCreateActor(Actor* in_actor) {
	CECompiler::OnCreateActor(in_actor);
}

void CECppCompiler::OnUpdateActor(Actor* in_actor, CETimestamp timestamp) {
	CECompiler::OnUpdateActor(in_actor, timestamp);
}

void CECppCompiler::OnPhysicsUpdate(Actor* in_actor) {
	CECompiler::OnPhysicsUpdate(in_actor);
}

void CECppCompiler::OnCollisionBegin(Actor* in_actor) {
	CECompiler::OnCollisionBegin(in_actor);
}

void CECppCompiler::OnCollisionEnd(Actor* in_actor) {
	CECompiler::OnCollisionEnd(in_actor);
}

void CECppCompiler::OnTriggerBegin(Actor* in_actor) {
	CECompiler::OnTriggerBegin(in_actor);
}

void CECppCompiler::OnTriggerEnd(Actor* in_actor) {
	CECompiler::OnTriggerEnd(in_actor);
}

bool CECppCompiler::IsActorModuleValid(Actor* actor) {
	return CECompiler::IsActorModuleValid(actor);
}

void CECppCompiler::OnScriptComponentDestroyed(CEUUID scene_id, CEUUID actor_id) {
	CECompiler::OnScriptComponentDestroyed(scene_id, actor_id);
}

bool CECppCompiler::ModuleExists(const std::string& module_name) {
	return CECompiler::ModuleExists(module_name);
}

void CECppCompiler::CreateActorScript(Actor* in_actor) {
	CECompiler::CreateActorScript(in_actor);
}

void CECppCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

void CECppCompiler::InstanceActorClass(Actor* in_actor) {
	CECompiler::InstanceActorClass(in_actor);
}

void CECppCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}
