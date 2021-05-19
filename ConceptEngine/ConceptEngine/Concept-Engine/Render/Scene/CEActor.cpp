#include "CEActor.h"
#include "CEScene.h"

ConceptEngine::Render::Scene::CEComponent::CEComponent(CEActor* actor): CEObject(), OwningActor(actor) {
	Assert(actor != nullptr);
	CORE_OBJECT_INIT();
}

ConceptEngine::Render::Scene::CEActor::CEActor(): CEObject(), Components(), Transform() {
	CORE_OBJECT_INIT();
}

ConceptEngine::Render::Scene::CEActor::~CEActor() {
	for (CEComponent* component : Components) {
		if (component) {
			delete (component);
			(component) = nullptr;
		}
	}

	Components.Clear();
}

void ConceptEngine::Render::Scene::CEActor::AddComponent(CEComponent* component) {
	Assert(component != nullptr);
	Components.EmplaceBack(component);
	if (Scene) {
		Scene->OnAddedComponent(component);
	}
}

void ConceptEngine::Render::Scene::CEActor::SetName(const std::string& name) {
	Name = name;
}
