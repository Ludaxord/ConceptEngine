#include <../ConceptEngineRenderer/Project/CEPlayground.h>

#include <../ConceptEngineRenderer/Scene/Camera.h>

class Sandbox : public CEPlayground
{
public:
    virtual bool Create() override;

    virtual void Update(CETimestamp DeltaTime) override;
private:
    Camera* CurrentCamera = nullptr;
    XMFLOAT3 CameraSpeed = XMFLOAT3(0.0f, 0.0f, 0.0f);

    int32 MouseOffsetX = 0;
    int32 MouseOffsetY = 0;
};