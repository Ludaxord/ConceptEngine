#include "CEPyCompiler.h"

//TODO: Implement...
bool CEPyCompiler::Create(const std::string& SourcePath) {
	return false;
}

//TODO: Implement...
void CEPyCompiler::Release() {
}

//TODO: Implement...
void CEPyCompiler::OnSceneDestruct(CEUUID SceneID) {
}

//TODO: Implement...
bool CEPyCompiler::LoadRuntimeScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
bool CEPyCompiler::LoadAppScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
bool CEPyCompiler::ReloadScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
void CEPyCompiler::SetSceneContext(const CEScene* scene) {
	CECompiler::SetSceneContext(scene);
}

//TODO: Implement...
const CEScene* CEPyCompiler::GetCurrentSceneContext() {
	return CECompiler::GetCurrentSceneContext();
}

//TODO: Implement...
void CEPyCompiler::OnCreateActor(Actor* in_actor) {
	CECompiler::OnCreateActor(in_actor);
}

//TODO: Implement...
void CEPyCompiler::OnUpdateActor(Actor* in_actor, CETimestamp timestamp) {
	CECompiler::OnUpdateActor(in_actor, timestamp);
}

//TODO: Implement...
void CEPyCompiler::OnPhysicsUpdate(Actor* in_actor) {
	CECompiler::OnPhysicsUpdate(in_actor);
}

//TODO: Implement...
void CEPyCompiler::OnCollisionBegin(Actor* in_actor) {
	CECompiler::OnCollisionBegin(in_actor);
}

//TODO: Implement...
void CEPyCompiler::OnCollisionEnd(Actor* in_actor) {
	CECompiler::OnCollisionEnd(in_actor);
}

//TODO: Implement...
void CEPyCompiler::OnTriggerBegin(Actor* in_actor) {
	CECompiler::OnTriggerBegin(in_actor);
}

//TODO: Implement...
void CEPyCompiler::OnTriggerEnd(Actor* in_actor) {
	CECompiler::OnTriggerEnd(in_actor);
}

//TODO: Implement...
bool CEPyCompiler::IsActorModuleValid(Actor* actor) {
	return CECompiler::IsActorModuleValid(actor);
}

//TODO: Implement...
void CEPyCompiler::OnScriptComponentDestroyed(CEUUID scene_id, CEUUID actor_id) {
	CECompiler::OnScriptComponentDestroyed(scene_id, actor_id);
}

//TODO: Implement...
bool CEPyCompiler::ModuleExists(const std::string& module_name) {
	return CECompiler::ModuleExists(module_name);
}

//TODO: Implement...
void CEPyCompiler::CreateActorScript(Actor* in_actor) {
	CECompiler::CreateActorScript(in_actor);
}

//TODO: Implement...
void CEPyCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

//TODO: Implement...
void CEPyCompiler::InstanceActorClass(Actor* in_actor) {
	CECompiler::InstanceActorClass(in_actor);
}

//TODO: Implement...
void CEPyCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}
