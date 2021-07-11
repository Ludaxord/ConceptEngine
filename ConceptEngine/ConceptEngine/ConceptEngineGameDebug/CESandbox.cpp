#include "CESandbox.h"

#include "Core/Platform/Generic/Managers/CECastManager.h"
#include "Render/Scene/CEActor.h"
#include "Render/Scene/CEScene.h"
#include "Render/Scene/Components/CEMeshComponent.h"

ConceptEngine::Graphics::Main::Common::CEPlayground* CreatePlayground() {
	return new CESandbox();
}

bool CESandbox::Create() {
	if (!CEPlayground::Create()) {
		return false;
	}

	ConceptEngine::Render::Scene::CEActor* Actor = nullptr;
	ConceptEngine::Render::Scene::Components::CEMeshComponent* MeshComponent = nullptr;
	Scene = ConceptEngine::Render::Scene::CEScene::LoadFromFile("");

	ConceptEngine::Graphics::Main::CEMeshData SphereMeshData = CastMeshManager()->CreateSphere(3);

	return true;
}

void CESandbox::Update(ConceptEngine::Time::CETimestamp DeltaTime) {
}
