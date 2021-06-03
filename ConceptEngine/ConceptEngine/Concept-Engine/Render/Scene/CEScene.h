#pragma once

#include <string>

#include "CECamera.h"
#include "CEActor.h"
#include "../../Time/CETimestamp.h"
#include "../../Core/Containers/CEArray.h"
#include "../../Core/Containers/CEContainerUtilities.h"
#include "../../Graphics/Main/Rendering/CEMeshDrawCommand.h"
#include "Lights/CELight.h"

namespace ConceptEngine::Render::Scene {
	class CEScene {
	public:
		CEScene();
		~CEScene();

		void Update(Time::CETimestamp deltaTime);

		void AddCamera(CECamera* camera);
		void AddActor(CEActor* actor);

		void OnAddedComponent(CEComponent* component);

		template <typename TComponent>
		const Core::Containers::CEArray<TComponent> GetAllComponentsOfType() const {
			Core::Containers::CEArray<TComponent> components;
			for (CEActor* actor : Actors) {
				TComponent* component = actor->GetComponentOfType<TComponent>();
				if (component) {
					components.EmplaceBack(*component);
				}
			}

			return Core::Containers::Move(components);
		}

		const Core::Containers::CEArray<CEActor*>& GetActors() const {
			return Actors;
		}

		const Core::Containers::CEArray<Lights::CELight*>& GetLights() const {
			return Lights;
		}

		const Core::Containers::CEArray<Graphics::Main::Rendering::CEMeshDrawCommand>& GetMeshDrawCommands() const {
			return MeshDrawCommands;
		}

		CECamera* GetCamera() const {
			return CurrentCamera;
		}

		static CEScene* LoadFromFile(const std::string& filePath);

	private:
		void AddMeshComponent(class CEMeshComponent* component);

		Core::Containers::CEArray<CEActor*> Actors;
		Core::Containers::CEArray<Lights::CELight*> Lights;
		Core::Containers::CEArray<Graphics::Main::Rendering::CEMeshDrawCommand> MeshDrawCommands;

		CECamera* CurrentCamera = nullptr;
	};
}
