#pragma once
#include "Actor.h"
#include "Camera.h"

#include "Lights/Light.h"

#include "../Rendering/MeshDrawCommand.h"

#include "../Time/CETimestamp.h"

#include "../Core/Containers/Array.h"

class CEScene
{
public:
    CEScene();
    ~CEScene();

    void Tick(CETimestamp DeltaTime);

    void AddCamera(Camera* InCamera);
    void AddActor(Actor* InActor);
    void AddLight(Light* InLight);

    void OnAddedComponent(Component* NewComponent);

    template<typename TComponent>
    FORCEINLINE const CEArray<TComponent> GetAllComponentsOfType() const
    {
        // TODO: Cache this result

        CEArray<TComponent> Components;
        for (Actor* Actor : Actors)
        {
            TComponent* Component = Actor->GetComponentOfType<TComponent>();
            if (Component)
            {
                Components.EmplaceBack(*Component);
            }
        }

        return Move(Components);
    }

    const CEArray<Actor*>& GetActors() const { return Actors; }
    const CEArray<Light*>& GetLights() const { return Lights; }

    const CEArray<MeshDrawCommand>& GetMeshDrawCommands() const { return MeshDrawCommands; }
     
    Camera* GetCamera() const { return CurrentCamera; }

    static CEScene* LoadFromFile(const std::string& Filepath);

private:
    void AddMeshComponent(class MeshComponent* Component);

    CEArray<Actor*> Actors;
    CEArray<Light*> Lights;
    CEArray<MeshDrawCommand> MeshDrawCommands;

    Camera* CurrentCamera = nullptr;
};