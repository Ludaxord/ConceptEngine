#pragma once
#include "RenderLayer/PipelineState.h"
#include "Scene/Actor.h"

class CEPhysicsComponent : Component {
public:
	CEPhysicsComponent() : CEPhysicsComponent(nullptr) {

	}

	explicit CEPhysicsComponent(Actor* InOwningActor) : Component(InOwningActor),
	                                                    TextureTransform(),
	                                                    FillMode(EFillMode::WireFrame),
	                                                    ReceiveShadow(false) {
		CORE_OBJECT_INIT();
	}

	TSharedPtr<class Material> Material;
	CERigidTransform MeshTransform;
	DirectX::XMFLOAT4X4 TextureTransform;
	EFillMode FillMode;
	bool ReceiveShadow;
};
