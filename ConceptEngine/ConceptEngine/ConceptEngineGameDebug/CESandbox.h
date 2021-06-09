#pragma once
#include "Graphics/Main/Common/CEPlayground.h"
#include "Render/Scene/CECamera.h"

class CESandbox : public ConceptEngine::Graphics::Main::Common::CEPlayground {
public:
	virtual bool Create() override;
	virtual void Update(ConceptEngine::Time::CETimestamp DeltaTime) override;
private:
	ConceptEngine::Render::Scene::CECamera* CurrentCamera = nullptr;
	DirectX::XMFLOAT3 CameraSpeed = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
};
