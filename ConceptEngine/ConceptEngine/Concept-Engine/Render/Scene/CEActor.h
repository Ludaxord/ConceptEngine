#pragma once

#include <string>

#include "../../Graphics/Main/Objects/CEObject.h"
#include "../../Core/Containers/CEArray.h"
#include "../../Graphics/Main/Common/CETransform.h"

namespace ConceptEngine::Render::Scene {
	class CEScene;
	class CEActor;

	class CEComponent : public Graphics::Main::Objects::CEObject {
	CORE_OBJECT(CEComponent, CEObject);

	public:
		CEComponent(CEActor* actor);
		virtual ~CEComponent() = default;

		CEActor* GetOwningActor() const {
			return OwningActor;
		}

	protected:
		CEActor* OwningActor = nullptr;
	};

	class CEActor : public Graphics::Main::Objects::CEObject {
	CORE_OBJECT(CEActor, CEObject);

	public:
		CEActor();
		~CEActor();

		void AddComponent(CEComponent* component);

		template <typename TComponent>
		bool HasComponentOfType() const noexcept {
			TComponent* result = nullptr;
			for (CEComponent* component : Components) {
				if (IsSubClassOf<TComponent>(component)) {
					return true;
				}
			}

			return false;
		}

		template <typename TComponent>
		TComponent* GetComponentOfType() const {
			for (CEComponent* component : Components) {
				if (IsSubClassOf<TComponent>(component)) {
					return static_cast<TComponent*>(component);
				}
			}
			return nullptr;
		}

		void OnAddedToScene(CEScene* scene) {
			Scene = scene;
		}

		void SetName(const std::string& name);

		void SetTransform(Graphics::Main::Common::CETransform& transform) {
			Transform = transform;
		}

		const std::string& GetName() const {
			return Name;
		}

		CEScene* GetScene() const {
			return Scene;
		}

		Graphics::Main::Common::CETransform& GetTransform() {
			return Transform;
		}

		const Graphics::Main::Common::CETransform& GetTransform() const {
			return Transform;
		}

	private:
		CEScene* Scene = nullptr;
		Graphics::Main::Common::CETransform Transform;
		CEArray<CEComponent*> Components;
		std::string Name;
	};
}
