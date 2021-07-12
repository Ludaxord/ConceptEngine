#include "CEScene.h"

using namespace ConceptEngine::Render::Scene;

CEScene::CEScene() {
}

CEScene::~CEScene() {
}

void CEScene::Update(Time::CETimestamp deltaTime) {
}

void CEScene::AddCamera(CECamera* camera) {
}

void CEScene::AddActor(CEActor* actor) {
}

void CEScene::OnAddedComponent(CEComponent* component) {
}

CEScene* CEScene::LoadFromFile(const std::string& filePath) {
	return nullptr;
}

void CEScene::AddMeshComponent(CEMeshComponent* component) {
}
