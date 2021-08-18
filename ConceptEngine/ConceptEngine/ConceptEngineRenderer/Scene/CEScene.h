#pragma once
#include "Actor.h"
#include "Camera.h"

#include "Lights/Light.h"

#include "../Rendering/MeshDrawCommand.h"

#include "../Time/CETimestamp.h"

#include "../Core/Containers/Array.h"
#include "Components/CEIDComponent.h"
#include "Components/CELinkComponent.h"

class CEScene {
public:
	CEScene();
	~CEScene();

	bool Create();
	void Tick(CETimestamp DeltaTime);
	void MoveActor(const Transform& MoveTransform, std::string MoveActorName);

	void AddCamera(Camera* InCamera);
	void AddActor(Actor* InActor);
	void AddLight(Light* InLight);

	void OnAddedComponent(Component* NewComponent);

	static CEScene* GetSceneByUUID(CEUUID UUID);

	template <typename TComponent>
	FORCEINLINE const CEArray<TComponent> GetAllComponentsOfType() const {
		// TODO: Cache this result

		CEArray<TComponent> Components;
		for (Actor* Actor : Actors) {
			TComponent* Component = Actor->GetComponentOfType<TComponent>();
			if (Component) {
				Components.EmplaceBack(*Component);
			}
		}

		return Move(Components);
	}

	template <typename TComponent>
	FORCEINLINE const CEArray<Actor*> GetActorsWithComponentsOfType() const {
		CEArray<Actor*> ActorsWithComponents;
		for (Actor* Actor : Actors) {
			TComponent* Component = Actor->GetComponentOfType<TComponent>();
			if (Component) {
				ActorsWithComponents.EmplaceBack(Actor);
			}
		}

		return Move(ActorsWithComponents);
	}

	const CEArray<Actor*>& GetActors() const {
		return Actors;
	}

	const CEArray<Light*>& GetLights() const {
		return Lights;
	}

	const CEArray<MeshDrawCommand>& GetMeshDrawCommands() const {
		return MeshDrawCommands;
	}

	Camera* GetCamera() const {
		return CurrentCamera;
	}

	Actor* GetActorByIndex(int32 Index) {
		return Actors[Index];
	}

	Actor* GetActorByUUID(CEUUID Uuid) {
		for (auto* Actor : Actors) {
			if (Actor->GetComponentOfType<CEIDComponent>()->ID == Uuid) {
				return Actor;
			}
		}

		return nullptr;
	}

	CEUUID GetUUID() const {
		return UUID;
	}

	static CEScene* LoadFromFile(const std::string& Filepath);

private:
	void AddMeshComponent(class CEMeshComponent* Component);

	CEArray<Actor*> Actors;
	CEArray<Light*> Lights;
	CEArray<MeshDrawCommand> MeshDrawCommands;

	Camera* CurrentCamera = nullptr;

	CEUUID UUID;
};
