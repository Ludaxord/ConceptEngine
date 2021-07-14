#include "CESandbox.h"

#include "Core/Platform/Generic/Managers/CECastManager.h"
#include "Render/Scene/CEActor.h"
#include "Render/Scene/CEScene.h"
#include "Render/Scene/Components/CEMeshComponent.h"

using namespace ConceptEngine::Render::Scene;
using namespace ConceptEngine::Graphics::Main;

Common::CEPlayground* CreatePlayground() {
	return new CESandbox();
}

bool CESandbox::Create() {
	if (!CEPlayground::Create()) {
		return false;
	}

	CEActor* Actor = nullptr;
	Components::CEMeshComponent* MeshComponent = nullptr;
	Scene = CEScene::LoadFromFile("");

	CEMeshData SphereMeshData = CastMeshManager()->CreateSphere(3);

	return true;
}

void CESandbox::Update(ConceptEngine::Time::CETimestamp DeltaTime) {
}
