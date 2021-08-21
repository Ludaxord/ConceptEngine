#pragma once
#include "Scene/Actor.h"

class CETransformComponent : public Component {
	CORE_OBJECT(CETransformComponent, Component);

public:
	CETransformComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}

	DirectX::XMFLOAT3 Translation = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3 Rotation = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3 Scale = {1.0f, 1.0f, 1.0f};

	DirectX::XMFLOAT3 Up = {0.0f, 1.0f, 0.0f};
	DirectX::XMFLOAT3 Right = {1.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3 Forward = {0.0f, 0.0f, -1.0f};

	XMFLOAT4X4& GetTransform() const {
		XMFLOAT4X4 Transform;
		XMStoreFloat4x4(
			&Transform,
			XMMatrixTranslationFromVector(XMLoadFloat3(&Translation)) *
			XMMatrixTranslationFromVector(XMLoadFloat3(&Rotation)) *
			XMMatrixScalingFromVector(XMLoadFloat3(&Scale))
		);
		return Transform;
	}
};
