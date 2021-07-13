#pragma once

#include <string>

#include "CECamera.h"
#include "CEActor.h"
#include "../../Time/CETimestamp.h"
#include "../../Core/Containers/CEArray.h"
#include "../../Core/Containers/CEContainerUtilities.h"
#include "../../Graphics/Main/Rendering/CEMeshDrawCommand.h"
#include "Lights/CELight.h"

namespace ConceptEngine::Render::Scene {namespace Components {
		class CEMeshComponent;
	}

	class CEScene {
	public:
		CEScene();
		~CEScene();

		void Update(Time::CETimestamp deltaTime);

		void AddCamera(CECamera* camera);
		void AddActor(CEActor* actor);
		void AddLight(Lights::CELight* Light);

		void OnAddedComponent(CEComponent* component);

		template <typename TComponent>
		const CEArray<TComponent> GetAllComponentsOfType() const {
			CEArray<TComponent> components;
			for (CEActor* actor : Actors) {
				TComponent* component = actor->GetComponentOfType<TComponent>();
				if (component) {
					components.EmplaceBack(*component);
				}
			}

			return Move(components);
		}

		const CEArray<CEActor*>& GetActors() const {
			return Actors;
		}

		const CEArray<Lights::CELight*>& GetLights() const {
			return Lights;
		}

		const CEArray<Graphics::Main::Rendering::CEMeshDrawCommand>& GetMeshDrawCommands() const {
			return MeshDrawCommands;
		}

		CECamera* GetCamera() const {
			return CurrentCamera;
		}

		static CEScene* LoadFromFile(const std::string& filePath);

	private:
		void AddMeshComponent(Components::CEMeshComponent* component);

		CEArray<CEActor*> Actors;
		CEArray<Lights::CELight*> Lights;
		CEArray<Graphics::Main::Rendering::CEMeshDrawCommand> MeshDrawCommands;

		CECamera* CurrentCamera = nullptr;
	};
}
