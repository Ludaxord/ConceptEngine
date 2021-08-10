#pragma once
#include "../Core/CoreObject/CoreObject.h"
#include "../Core/Containers/Array.h"
#include "../Physics/Transform/CERigidTransform.h"
#include "../Time/CETimestamp.h"

class Actor;

// Component BaseClass
class Component : public CoreObject
{
    CORE_OBJECT(Component, CoreObject);

public:
    Component(Actor* InOwningActor);
    virtual ~Component() = default;

    Actor* GetOwningActor() const { return OwningActor; }

protected:
    Actor* OwningActor = nullptr;
};

class Transform
{
public:
    Transform();
    ~Transform() = default;

    void SetTranslation(float x, float y, float z);
    void SetTranslation(const XMFLOAT3& InPosition);

    void SetScale(float x, float y, float z);
    void SetScale(const XMFLOAT3& InScale);

    void SetUniformScale(float InScale)
    {
        SetScale(InScale, InScale, InScale);
    }

    void SetRotation(float x, float y, float z);
    void SetRotation(const XMFLOAT3& InRotation);

    const XMFLOAT3& GetTranslation() const { return Translation; }

    const XMFLOAT3& GetScale() const { return Scale; }

    const XMFLOAT3& GetRotation() const { return Rotation; }

    const XMFLOAT4X4& GetMatrix() const { return Matrix; }
    const XMFLOAT4X4& GetMatrixInverse() const { return MatrixInv; }

    const XMFLOAT3X4& GetTinyMatrix() const { return TinyMatrix; }
    CERigidTransform AsRigidTransform() const;

    //TODO: Add mapping to CERigidTransform...

private:
    void CalculateMatrix();

    XMFLOAT4X4 Matrix;
    XMFLOAT4X4 MatrixInv;
    XMFLOAT3X4 TinyMatrix;
    XMFLOAT3   Translation;
    XMFLOAT3   Scale;
    XMFLOAT3   Rotation;
};

class CEScene;

class Actor : public CoreObject
{
    CORE_OBJECT(Actor, CoreObject);

public:
    Actor();
    ~Actor();

    void AddComponent(Component* InComponent);

    template<typename TComponent>
    FORCEINLINE bool HasComponentOfType() const noexcept
    {
        TComponent* Result = nullptr;
        for (Component* Component : Components)
        {
            if (IsSubClassOf<TComponent>(Component))
            {
                return true;
            }
        }

        return false;
    }

    template <typename TComponent>
    FORCEINLINE TComponent* GetComponentOfType() const
    {
        for (Component* Component : Components)
        {
            if (IsSubClassOf<TComponent>(Component))
            {
                return static_cast<TComponent*>(Component);
            }
        }

        return nullptr;
    }

    void OnAddedToScene(CEScene* InScene)
    {
        Scene = InScene;
    }
    
    void SetName(const std::string& InDebugName);

    void SetTransform(const Transform& InTransform)
    {
        Transform = InTransform;
    }

    const std::string& GetName() const { return Name; }

    CEScene* GetScene() const { return Scene; }

    Transform& GetTransform() { return Transform; }
    const Transform& GetTransform() const { return Transform; }
    void Update(CETimestamp DeltaTime) override;

private:
    CEScene*    Scene = nullptr;
    Transform Transform;
    CEArray<Component*> Components;
    std::string        Name;
};