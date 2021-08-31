#include "CEGraphsCompiler.h"

//TODO: Implement...
bool CEGraphsCompiler::Create(const std::string& SourcePath) {
	return false;
}

//TODO: Implement...
void CEGraphsCompiler::Release() {
}

//TODO: Implement...
void CEGraphsCompiler::OnSceneDestruct(CEUUID SceneID) {
}

//TODO: Implement...
bool CEGraphsCompiler::LoadRuntimeScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
bool CEGraphsCompiler::LoadAppScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
bool CEGraphsCompiler::ReloadScript(const std::string& ScriptPath) {
	return false;
}

//TODO: Implement...
void CEGraphsCompiler::SetSceneContext(const CEScene* scene) {
	CECompiler::SetSceneContext(scene);
}

//TODO: Implement...
const CEScene* CEGraphsCompiler::GetCurrentSceneContext() {
	return CECompiler::GetCurrentSceneContext();
}

//TODO: Implement...
void CEGraphsCompiler::OnCreateActor(Actor* in_actor) {
	CECompiler::OnCreateActor(in_actor);
}

//TODO: Implement...
void CEGraphsCompiler::OnUpdateActor(Actor* in_actor, CETimestamp timestamp) {
	CECompiler::OnUpdateActor(in_actor, timestamp);
}

//TODO: Implement...
void CEGraphsCompiler::OnPhysicsUpdate(Actor* in_actor) {
	CECompiler::OnPhysicsUpdate(in_actor);
}

//TODO: Implement...
void CEGraphsCompiler::OnCollisionBegin(Actor* in_actor) {
	CECompiler::OnCollisionBegin(in_actor);
}

//TODO: Implement...
void CEGraphsCompiler::OnCollisionEnd(Actor* in_actor) {
	CECompiler::OnCollisionEnd(in_actor);
}

//TODO: Implement...
void CEGraphsCompiler::OnTriggerBegin(Actor* in_actor) {
	CECompiler::OnTriggerBegin(in_actor);
}

//TODO: Implement...
void CEGraphsCompiler::OnTriggerEnd(Actor* in_actor) {
	CECompiler::OnTriggerEnd(in_actor);
}

//TODO: Implement...
bool CEGraphsCompiler::IsActorModuleValid(Actor* actor) {
	return CECompiler::IsActorModuleValid(actor);
}

//TODO: Implement...
void CEGraphsCompiler::OnScriptComponentDestroyed(CEUUID scene_id, CEUUID actor_id) {
	CECompiler::OnScriptComponentDestroyed(scene_id, actor_id);
}

//TODO: Implement...
bool CEGraphsCompiler::ModuleExists(const std::string& module_name) {
	return CECompiler::ModuleExists(module_name);
}

//TODO: Implement...
void CEGraphsCompiler::CreateActorScript(Actor* in_actor) {
	CECompiler::CreateActorScript(in_actor);
}

//TODO: Implement...
void CEGraphsCompiler::ReleaseActorScript(Actor* in_actor, const std::string& module_name) {
	CECompiler::ReleaseActorScript(in_actor, module_name);
}

//TODO: Implement...
void CEGraphsCompiler::InstanceActorClass(Actor* in_actor) {
	CECompiler::InstanceActorClass(in_actor);
}

//TODO: Implement...
void CEGraphsCompiler::OnEditorRender() {
	CECompiler::OnEditorRender();
}
