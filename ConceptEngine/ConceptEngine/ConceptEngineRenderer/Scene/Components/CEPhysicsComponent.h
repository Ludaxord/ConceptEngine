#pragma once
#include "../Rendering/Resources/Material.h"
#include "../RenderLayer/PipelineState.h"
#include "../Scene/Actor.h"

class CEPhysicsComponent : Component {
	CORE_OBJECT(CEPhysicsComponent, Component);

public:
	CEPhysicsComponent() : CEPhysicsComponent(nullptr) {

	}

	explicit CEPhysicsComponent(Actor* InOwningActor) : Component(InOwningActor),
	                                                    TextureTransform(),
	                                                    FillMode(EFillMode::WireFrame),
	                                                    ReceiveShadow(false) {
		CORE_OBJECT_INIT();
	}

	TSharedPtr<Material> Material;
	CERigidTransform MeshTransform;
	DirectX::XMFLOAT4X4 TextureTransform;
	EFillMode FillMode;
	bool ReceiveShadow;
};
